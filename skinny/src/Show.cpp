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

  for (auto& fbo : fbos_)
  {
    fbo.allocate(s);

    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
  }

  if (currentScene_)
    currentScene_->init();

#ifdef TARGET_WIN32
  spoutSender_.init(NAME, fbos_[2].getTexture());
#endif
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
    fbos_[0].begin();
    ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    firstPassShader_.begin();
    setupFirstPassUniforms();
    currentScene_->bind();
    ofDrawRectangle(0, 0, width_, height_);
    currentScene_->unbind();
    firstPassShader_.end();
    fbos_[0].end();

		// second pass
		fbos_[1].begin();
		pingPongPassShader_.begin();
		setupPingPongPassUniforms(true, fbos_[0].getTexture());
		ofDrawRectangle(0, 0, width_, height_);
		pingPongPassShader_.end();
		fbos_[1].end();

    // third pass
    fbos_[2].begin();
		pingPongPassShader_.begin();
		setupPingPongPassUniforms(false, fbos_[1].getTexture());
		ofDrawRectangle(0, 0, width_, height_);
		pingPongPassShader_.end();
    fbos_[2].end();

    // draw
    fbos_[2].getTexture().draw(0.f, 0.f);
}

//--------------------------------------------------------------
bool Show::reload(const ShowDescription& description)
{
    masterAlphaControl_ = description.getAlphaControl();

    return reloadLayers(description) && reloadEffects(description);
}

//--------------------------------------------------------------
bool Show::reloadLayers(const ShowDescription& description)
{
	currentScene_->bind();
	firstPassShader_.begin();

  currentScene_->reload(description.currentScene());

	firstPassShader_.end();
	currentScene_->unbind();

	return currentScene_->isValid();
}

//--------------------------------------------------------------
bool Show::reloadEffects(const ShowDescription& description)
{
	for (auto i = 0; i < MAX_EFFECTS; ++i)
  {
		const auto& effect = description.getEffects()[i];

    if (effects_[i])
    {
      if (effects_[i]->type == effect.type)
      {
        continue;
      }
      else
      {
        effects_[i]->exit();
      }
    }

    effects_[i].reset(new Effect(i, effect.type, effect.note, effect.cc, effect.param));
    effects_[i]->setup();
	}

  return true;
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

#ifdef TARGET_WIN32
    spoutSender_.send(fbos_[2].getTexture());
#endif
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
    const auto screenSize = glm::ivec2{ width_, height_ };
    const auto* screenSizeArr = reinterpret_cast<const int*>(&screenSize);
    firstPassShader_.setUniform2iv("screenSize", screenSizeArr);

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
