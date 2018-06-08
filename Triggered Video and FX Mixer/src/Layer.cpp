#include "Layer.h"

Layer::Layer(int id)
    : id_(id), Mappable()
{
    player_.setPixelFormat(OF_PIXELS_BGRA);
    player_.setLoopState(OF_LOOP_NORMAL);
}

Layer::Layer(int id, const std::string& filename)
    : Layer(id)
{
    reload(filename);
}

Layer::Layer(int id, const std::string & filename, const MidiMap & map)
    : id_(id), Mappable(map)
{
    player_.setPixelFormat(OF_PIXELS_BGRA);
    player_.setLoopState(OF_LOOP_NORMAL);
    reload(filename);    
}

Layer::~Layer()
{   
    //player_.getTexture().unbind(id_); // throws exception renderer being null
    player_.closeMovie();
}

bool Layer::reload(const std::string& filename)
{
    player_.closeMovie();

    if (!player_.load(filename)) {
        ofLog(OF_LOG_ERROR, "Cannot load %s.", filename);
        return false;
    }

    return true;
}

void Layer::bind() {
    player_.getTexture().bindAsImage(id_, GL_READ_ONLY);
}

void Layer::play()
{
    active_ = true;
    player_.setPaused(!active_);
}

void Layer::pause()
{
    active_ = false;
    player_.setPaused(!active_);
}

void Layer::playPause()
{
    active_ = !active_;
    player_.setPaused(!active_);
}

bool Layer::isFrameNew()
{
    player_.update();
    return player_.isFrameNew(); 
}
