#include "Show.h"
#include "Status.h"

#include <chrono>
#include <ctime>

namespace skinny {

//--------------------------------------------------------------
Show::Show(int width, int height) :
    width_(width),
    height_(height),
    currentScene_(std::make_shared<Scene>())
{
	if (!loadShaders()) {
		ofLog(OF_LOG_FATAL_ERROR, "Could not load shaders.");
		Status::instance().exit = true;
		return;
	}
}

//--------------------------------------------------------------
bool Show::loadShaders()
{
  std::string shaderPathPrefix;

#ifndef NDEBUG
  shaderPathPrefix = "../../src/shaders/";
#else
  shaderPathPrefix = "shaders/";
#endif

	firstPassShader_.load(shaderPathPrefix + "firstPassShader");
  pingPongPassShader_.load(shaderPathPrefix + "pingPongPassShader");
	
  return firstPassShader_.isLoaded() && pingPongPassShader_.isLoaded();
}

//--------------------------------------------------------------
void Show::setup()
{
  Mappable::setup();

	ofFboSettings s;
	s.width = width_;
	s.height = height_;
	s.internalformat = GL_RGBA;
	s.useDepth = false;

  firstPassFbo_.allocate(s);
  secondPassFbo_.allocate(s);

  firstPassFbo_.begin();
	ofClear(255, 255, 255, 0);
	firstPassFbo_.end();

  secondPassFbo_.begin();
	ofClear(255, 255, 255, 0);
  secondPassFbo_.end();

  if (currentScene_)
    currentScene_->init();

  spoutSender_.init(NAME, secondPassFbo_.getTexture());
}

//--------------------------------------------------------------
void Show::exit()
{
  Mappable::exit();

  if (currentScene_)
    currentScene_->done();

  for (auto& effect : effects_)
  {
    if (effect)
      effect->exit();
  }
}

//--------------------------------------------------------------
void Show::draw()
{
    // Make sure the first frame is ready before we start drawing.
    static bool firstFrameReady = false;
    firstFrameReady = firstFrameReady || currentScene_->isFrameNew();

    if (!firstFrameReady)
        return;

    // first pass
    firstPassFbo_.begin();
    ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    firstPassShader_.begin();
    setupFirstPassUniforms();
    currentScene_->bind();
    ofDrawRectangle(0, 0, width_, height_);
    currentScene_->unbind();
    firstPassShader_.end();
    firstPassFbo_.end();

		// second pass
		secondPassFbo_.begin();
		pingPongPassShader_.begin();
		setupPingPongPassUniforms(true, firstPassFbo_.getTexture());
		ofDrawRectangle(0, 0, width_, height_);
		pingPongPassShader_.end();
		secondPassFbo_.end();

    // third pass
		pingPongPassShader_.begin();
		setupPingPongPassUniforms(false, secondPassFbo_.getTexture());
		ofDrawRectangle(0, 0, width_, height_);
		pingPongPassShader_.end();
}

//--------------------------------------------------------------
bool Show::reload(const ShowDescription& description)
{
    masterAlphaControl_ = description.getAlphaControl();

    currentScene_->bind();

    firstPassShader_.begin();
    currentScene_->reload(description.currentScene());

    for (auto i = 0; i < MAX_EFFECTS; ++i) {
       if (effects_[i])
          effects_[i]->exit();

        const auto& effect = description.getEffects()[i];
        effects_[i].reset(new Effect(i, effect.type, effect.note, effect.cc, effect.param));
        effects_[i]->setup();
    }

    firstPassShader_.end();
    currentScene_->unbind();

    return currentScene_->isValid();
}

//--------------------------------------------------------------
void Show::playPauseEffect(int i)
{
    effects_[i]->playPause();
}

//--------------------------------------------------------------
void Show::update()
{
    if (currentScene_) {
        currentScene_->update();
    }

    spoutSender_.send(secondPassFbo_.getTexture());
}

//--------------------------------------------------------------
void Show::onNoteOn(NoteMessage& msg)
{
  if (Mappable::isCorrectChannel(msg.channel_) &&
      msg.note_ == getStatus().showDescription->getSwitchNote())
  {
    Status::instance().loadDir = LoadDir::Forward;
  }
}

//--------------------------------------------------------------
void Show::onControlChange(ControlChangeMessage& msg)
{
	if (Mappable::isCorrectChannel(msg.channel_) &&
			msg.control_ == masterAlphaControl_)
  {
    uniforms_.masterAlpha_ = clamp(msg.value_, 0, MAX_7BIT) / MAX_7BITF;
  }
}

//--------------------------------------------------------------
ScenePtr Show::getCurrentScene() const
{
    return currentScene_;
}

//--------------------------------------------------------------
const Show::Effects& Show::getEffects() const
{
    return effects_;
}

//--------------------------------------------------------------
midiNote Show::getAlphaControl() const
{
    return masterAlphaControl_;
}

//--------------------------------------------------------------
float Show::getAlpha() const
{
    return uniforms_.masterAlpha_;
}

//--------------------------------------------------------------
void Show::setAlphaControl(const midiNote & control)
{
    masterAlphaControl_ = control;
}
//--------------------------------------------------------------
float getTimeshift()
{
  using namespace std::chrono;
  return std::fmod(duration<float>(steady_clock::now().time_since_epoch()).count(), 2.0f * 3.1459f);
}

//--------------------------------------------------------------
void Show::setupFirstPassUniforms() const
{
    currentScene_->setupUniforms(firstPassShader_);

    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        uniforms_.fxTypes[i] = static_cast<int>(effects_[i]->type);
        uniforms_.fxPlaying[i] = effects_[i]->isPlaying();
        uniforms_.fxParam[i] = effects_[i]->getParam() / MAX_7BITF;
    }

    firstPassShader_.setUniform1f("masterAlpha", uniforms_.masterAlpha_);
    firstPassShader_.setUniform1f("timeShift", getTimeshift());
    firstPassShader_.setUniform2iv("screenSize", reinterpret_cast<int*>(&glm::ivec2{ width_, height_ }));

    firstPassShader_.setUniform1iv("fxTypes", uniforms_.fxTypes, MAX_EFFECTS);
    firstPassShader_.setUniform1iv("fxPlaying", uniforms_.fxPlaying, MAX_EFFECTS);
    firstPassShader_.setUniform1fv("fxParam", uniforms_.fxParam, MAX_EFFECTS);
}

//--------------------------------------------------------------
void Show::setupPingPongPassUniforms(bool horizontal, const ofTexture& img) const
{
	// keep effect uniforms from first pass
	pingPongPassShader_.setUniform1iv("fxTypes", uniforms_.fxTypes, MAX_EFFECTS);
	pingPongPassShader_.setUniform1iv("fxPlaying", uniforms_.fxPlaying, MAX_EFFECTS);
	pingPongPassShader_.setUniform1fv("fxParam", uniforms_.fxParam, MAX_EFFECTS);
	pingPongPassShader_.setUniform1i("horizontal", horizontal);
	pingPongPassShader_.setUniformTexture("previousPass", img, 1);
}

//--------------------------------------------------------------
bool Show::hasActiveFX() const
{
    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        if (uniforms_.fxPlaying[i]) {
            return true;
        }
    }

    return false;
}

} // namespace skinny
