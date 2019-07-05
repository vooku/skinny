#include "Show.h"
#include "Status.h"

Show::Show(ofShader& shader, int width, int height) :
    shader_(shader),
    width_(width),
    height_(height),
    currentScene_(std::make_unique<Scene>())
{}

void Show::update()
{
    if (!currentScene_)
        return;

    if (currentScene_->isFrameNew() || Status::instance().redraw) {
        shader_.begin();
        currentScene_->setupUniforms(shader_);
        shader_.dispatchCompute(width_ / 32, height_ / 32, 1);
        shader_.end();
        Status::instance().redraw = currentScene_->hasActiveFX();
    }
}

void Show::newMidiMessage(ofxMidiMessage& msg)
{
    auto foundMappables = currentScene_->newMidiMessage(msg);
    for (const auto& layer : foundMappables.layers) {
        //gui_.setActive(layer.first, layer.second);
    }
    for (const auto& effect : foundMappables.effects) {
        //gui_.setActive(effect.first, effect.second);
    }

    Status::instance().redraw = true;
}

bool Show::reload(const SceneDescription& description)
{
    shader_.begin();
    currentScene_->reload(description);
    currentScene_->bindTextures();
    shader_.end();

    if (currentScene_->isValid()) {
        ofLog(OF_LOG_NOTICE, "Successfully loaded scene %s.", currentScene_->getName().c_str());
        //gui_.reload(currentScene_.get());
    }
    else {
        // TODO display in gui
        ofLog(OF_LOG_WARNING, "Scene %s encountered loading problems.", currentScene_->getName().c_str());
    }

    return currentScene_->isValid();
}
