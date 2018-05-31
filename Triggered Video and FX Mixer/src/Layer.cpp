#include "Layer.h"

Layer::Layer(int id)
    : id_(id), paused_(true)
{
    player_.setPixelFormat(OF_PIXELS_BGRA);
    player_.setLoopState(OF_LOOP_NORMAL);
}

Layer::Layer(int id, const char * filename)
    : Layer(id)
{
    reload(filename);
}

Layer::~Layer()
{
    player_.closeMovie();
}

bool Layer::reload(const char * filename)
{
    player_.closeMovie();

    if (!player_.load(filename)) {
        ofLog(OF_LOG_ERROR, "Cannot load %s.", filename);
        return false;
    }

    player_.getTexture().bindAsImage(id_, GL_READ_ONLY);

    return true;
}

void Layer::play()
{
    paused_ = false;
    player_.setPaused(paused_);
    globalAlpha_ = 1.0f;
}

void Layer::pause()
{
    paused_ = true;
    player_.setPaused(paused_);
    globalAlpha_ = 0.0f;
}

void Layer::playPause()
{
    paused_ = !paused_;
    player_.setPaused(paused_);
    globalAlpha_ = 1.0f - globalAlpha_;
}

bool Layer::update()
{
    player_.update();
    return player_.isFrameNew(); 
}