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

    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        if (effects_[i]->getNote() == note) {
            if (noteOn) {
                effects_[i]->play();
                result.effects.emplace_back(i, true);
            }
            else if (noteOff) {
                effects_[i]->pause();
                result.effects.emplace_back(i, false);
            }
        }
    }

    return result;
}

bool Show::reload(const ShowDescription& description)
{
    shader_.begin();
    currentScene_->reload(description.currentScene());
    currentScene_->bindTextures();

    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        const auto& effect = description.getEffects()[i];
        effects_[i].reset(new Effect(effect.type, effect.note));
    }

    shader_.end();

    return currentScene_->isValid();
}

void Show::playPauseEffect(int i)
{
    effects_[i]->playPause();
}

void Show::setupUniforms() const
{
    currentScene_->setupUniforms(shader_);

    for (auto i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
        effectUniforms_[static_cast<Effect::Type>(i)] = false;
    }

    for (const auto& effect : effects_) {
        effectUniforms_[effect->type] = effect->isPlaying() || effectUniforms_[effect->type];
    }

    shader_.setUniform1i("invert", effectUniforms_[Effect::Type::Inverse]);
    shader_.setUniform1i("reducePalette", effectUniforms_[Effect::Type::ReducePalette]);
    shader_.setUniform1i("colorShift", effectUniforms_[Effect::Type::ColorShift]);
    shader_.setUniform1i("colorShift2", effectUniforms_[Effect::Type::ColorShift2]);
}

bool Show::hasActiveFX() const
{
    for (const auto& fu : effectUniforms_) {
        if (fu.second) {
            return true;
        }
    }

    return false;
}
