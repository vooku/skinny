#include "Layer.h"

char * Layer::c_str(BlendMode blendMode)
{
    switch (blendMode)
    {
    case BlendMode::Normal:
        return "Normal";
    case BlendMode::Multiply:
        return "Multiply";
    case BlendMode::Screen:
        return "Screen";
    case BlendMode::Darken:
        return "Darken";
    case BlendMode::Lighten:
        return "Lighten";
    case BlendMode::LinearDodge:
        return "LinearDodge";
    case BlendMode::Difference:
        return "Difference";
    case BlendMode::Exclusion:
        return "Exclusion";
    default:
        return "Invalid";
    }
}

Layer::Layer(int id, const std::filesystem::path& path, midiNote note) :
    Mappable(note == -1 ? MIDI_OFFSET + id : note),
    id_(id),
    name_(path.filename().string()),
    valid_(false),
    alpha_(1),
    alphaControl_(ALPHA_MIDI_OFFSET + id)
{
    player_.setPixelFormat(OF_PIXELS_BGRA);
    player_.setLoopState(OF_LOOP_NORMAL);
    player_.setVolume(0);

    valid_ = reload(path);

    if (!valid_) {
        ofLog(OF_LOG_ERROR, "Cannot load %s at %s", name_.c_str(), path.c_str());
    }
    else
        ofLog(OF_LOG_VERBOSE, "Loaded %s.", name_.c_str());
}

Layer::Layer(int id, ErrorType error) :
    Mappable(MIDI_OFFSET + id),
    id_(id),
    name_(error == ErrorType::Invalid ? "Invalid description." : "Failed to load."),
    valid_(false),
    alpha_(0),
    alphaControl_(ALPHA_MIDI_OFFSET + id)
{
}

Layer::~Layer()
{
    //player_.getTexture().unbind(id_); // throws exception renderer being null
    player_.closeMovie();
}

bool Layer::reload(const std::filesystem::path& path)
{
    player_.closeMovie();
    return player_.load(path.string());
}

void Layer::bind() {
    if (!valid_)
        return;
    player_.getTexture().bindAsImage(id_, GL_READ_ONLY);
}

void Layer::play()
{
    active_ = !mute_;
    player_.setPaused(!active_);
}

void Layer::pause()
{
    active_ = false;
    if (retrigger_)
        player_.setFrame(0);
    player_.setPaused(!active_);
}

void Layer::playPause()
{
    active_ = !active_ && !mute_;
    if (!active_ && retrigger_)
        player_.setFrame(0);
    player_.setPaused(!active_);
}

bool Layer::isFrameNew()
{
    player_.update();
    return player_.isFrameNew();
}
