#include "Layer.h"

char * Layer::c_str(BlendMode blendMode)
{
    switch (blendMode)
    {
    case Layer::BlendMode::Normal:
        return "Normal";
    case Layer::BlendMode::Multiply:
        return "Multiply";
    case Layer::BlendMode::Screen:
        return "Screen";
    case Layer::BlendMode::Darken:
        return "Darken";
    case Layer::BlendMode::Lighten:
        return "Lighten";
    case Layer::BlendMode::LinearDodge:
        return "LinearDodge";
    case Layer::BlendMode::Difference:
        return "Difference";
    case Layer::BlendMode::Exclusion:
        return "Exclusion";
    default:
        return "Invalid";
    }
}

Layer::Layer(int id, const std::filesystem::path& path, const MidiMap & map) :
    id_(id),
    name_(path.filename().string()),
    Mappable(map),
    valid_(false)
{
    player_.setPixelFormat(OF_PIXELS_BGRA);
    player_.setLoopState(OF_LOOP_NORMAL);
    reload(path);    
}

Layer::~Layer()
{   
    //player_.getTexture().unbind(id_); // throws exception renderer being null
    player_.closeMovie();
}

bool Layer::reload(const std::filesystem::path& path)
{
    player_.closeMovie();
    valid_ = player_.load(path.string());
    player_.setVolume(0);
    
    if (!valid_) {
        ofLog(OF_LOG_ERROR, "Cannot load %s at %s", name_.c_str(), path.c_str());
    }
    else 
        ofLog(OF_LOG_VERBOSE, "Loaded %s.", name_.c_str());

    return valid_;
}

void Layer::bind() {
    if (!valid_)
        return;
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
