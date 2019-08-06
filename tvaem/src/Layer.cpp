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

Layer::Layer(int id, const std::filesystem::path& path, midiNote note, midiNote control) :
    Mappable(note == -1 ? MIDI_OFFSET + id : note,
             control == -1 ? ALPHA_MIDI_OFFSET + id : note),
    id_(id),
    name_(path.filename().string()),
    valid_(false),
    alpha_(1)
{
    player_.setPixelFormat(OF_PIXELS_BGRA);
    player_.setLoopState(OF_LOOP_NORMAL);

    valid_ = reload(path);

    if (!valid_) {
        ofLog(OF_LOG_ERROR, "Cannot load %s at %s", name_.c_str(), path.c_str());
    }
    else
        ofLog(OF_LOG_VERBOSE, "Loaded %s.", name_.c_str());
}

Layer::Layer(int id, ErrorType error) :
    Mappable(MIDI_OFFSET + id, ALPHA_MIDI_OFFSET + id),
    id_(id),
    name_(error == ErrorType::Invalid ? "Invalid description." : "Failed to load."),
    valid_(false),
    blendMode_(BlendMode::Normal),
    alpha_(0)
{
}

Layer::~Layer()
{
    player_.closeMovie();
}

bool Layer::reload(const std::filesystem::path& path)
{
    player_.closeMovie();
    const auto success = player_.load(path.string());
    player_.setVolume(0);
    return success;
}

void Layer::bind() {
    if (!valid_)
        return;
    player_.getTexture().bind(id_);
}


void Layer::unbind()
{
    if (!valid_)
        return;
    player_.getTexture().unbind(id_);
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
