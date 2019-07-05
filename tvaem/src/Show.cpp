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
        currentScene_->setupUniforms(shader_);
        shader_.dispatchCompute(width_ / 32, height_ / 32, 1);
        shader_.end();
        Status::instance().redraw = currentScene_->hasActiveFX();
    }
}

void Show::draw()
{
    dst_.draw(0, 0);
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
