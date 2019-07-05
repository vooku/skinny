#include "Show.h"
#include "Status.h"

Show::Show(ofShader& shader, int width, int height) :
    shader_(shader),
    width_(width),
    height_(height),
    currentScene_(std::make_shared<Scene>())
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

Scene::FoundMappables Show::newMidiMessage(ofxMidiMessage& msg)
{
    return currentScene_->newMidiMessage(msg);
}

bool Show::reload(const SceneDescription& description)
{
    shader_.begin();
    currentScene_->reload(description);
    currentScene_->bindTextures();
    shader_.end();

    return currentScene_->isValid();
}
