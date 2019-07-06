#include "Show.h"
#include "Status.h"

Show::Show(int width, int height) :
    width_(width),
    height_(height),
    currentScene_(std::make_shared<Scene>())
{
#ifndef NDEBUG
    if (!shader_.setupShaderFromFile(GL_COMPUTE_SHADER, "../../src/shader.comp")) {
#else
    if (!shader_.setupShaderFromFile(GL_COMPUTE_SHADER, "shader.comp")) {
#endif
        ofLog(OF_LOG_FATAL_ERROR, "Could not load shader.");
        Status::instance().exit = true;
        return;
    }
    if (!shader_.linkProgram()) {
        ofLog(OF_LOG_FATAL_ERROR, "Could not link shader.");
        Status::instance().exit = true;
        return;
    }

    dst_.allocate(width_, height_, GL_RGBA8);
    dst_.bindAsImage(7, GL_WRITE_ONLY);
}

void Show::update()
{
    if (!currentScene_)
        return;

    if (currentScene_->isFrameNew() || Status::instance().redraw) {
        shader_.begin();
        setupUniforms();
        shader_.dispatchCompute(width_ / 32, height_ / 32, 1);
        shader_.end();
        Status::instance().redraw = hasActiveFX();
    }
}

void Show::draw()
{
    dst_.draw(0, 0);
}

Scene::FoundMappables Show::newMidiMessage(ofxMidiMessage& msg)
{
    auto result = currentScene_->newMidiMessage(msg);

    const auto noteOn = msg.status == MIDI_NOTE_ON;
    const auto noteOff = msg.status == MIDI_NOTE_OFF;
    const auto note = msg.pitch;

    for (auto& effect : effects_) {
        if (effect.second.getNote() == note) {
            if (noteOn) {
                effect.second.play();
                result.effects.insert({ effect.first, true });
            }
            else if (noteOff) {
                effect.second.pause();
                result.effects.insert({ effect.first, false });
            }
        }
    }

    return result;
}

bool Show::reload(const SceneDescription& description)
{
    shader_.begin();
    currentScene_->reload(description);
    currentScene_->bindTextures();

    for (const auto& effect : description.effects) {
        effects_[effect.type] = Effect(effect.note);
    }

    shader_.end();

    return currentScene_->isValid();
}

void Show::playPauseEffect(Effect::Type type)
{
    effects_.at(type).playPause();
}

void Show::setupUniforms() const
{
    currentScene_->setupUniforms(shader_);

    auto it = effects_.find(Effect::Type::Inverse);
    if (it != effects_.end())
        uniforms_.inverse = it->second.isPlaying();
    it = effects_.find(Effect::Type::ReducePalette);
    if (it != effects_.end())
        uniforms_.reducePalette = it->second.isPlaying();
    it = effects_.find(Effect::Type::ColorShift);
    if (it != effects_.end())
        uniforms_.colorShift = it->second.isPlaying();
    it = effects_.find(Effect::Type::ColorShift2);
    if (it != effects_.end())
        uniforms_.colorShift2 = it->second.isPlaying();

    shader_.setUniform1i("invert", uniforms_.inverse);
    shader_.setUniform1i("reducePalette", uniforms_.reducePalette);
    shader_.setUniform1i("colorShift", uniforms_.colorShift);
    shader_.setUniform1i("colorShift2", uniforms_.colorShift2);
}

bool Show::hasActiveFX() const
{
    return uniforms_.inverse || uniforms_.reducePalette || uniforms_.colorShift || uniforms_.colorShift2;
}
