#include "Layer.h"

namespace skinny {

//--------------------------------------------------------------
Layer::Layer(int id, const std::filesystem::path& path, midiNote note, midiNote control) :
    Playable(note == -1 ? LAYER_NOTE_OFFSET + id : note,
             control == -1 ? LAYER_CC_OFFSET + id : note),
    id_(id),
    name_(path.filename().string()),
    valid_(false)
{
    player_.setPixelFormat(OF_PIXELS_BGRA);
    player_.setLoopState(OF_LOOP_NORMAL);

    ccValue_ = MAX_7BIT;

    valid_ = reload(path);

    if (!valid_) {
        ofLog(OF_LOG_ERROR, "Cannot load %s at %s", name_.c_str(), path.c_str());
    }
    else
        ofLog(OF_LOG_VERBOSE, "Loaded %s.", name_.c_str());
}

//--------------------------------------------------------------
Layer::Layer(int id, ErrorType error) :
    Playable(LAYER_NOTE_OFFSET + id, LAYER_CC_OFFSET + id),
    id_(id),
    name_(error == ErrorType::Invalid ? "Invalid description." : "Failed to load."),
    valid_(false),
    blendMode_(DEFAULT_BLEND_MODE)
{
}

//--------------------------------------------------------------
Layer::~Layer()
{
    player_.closeMovie();
}

//--------------------------------------------------------------
bool Layer::reload(const std::filesystem::path& path)
{
    player_.closeMovie();
    const auto success = player_.load(path.string());
    player_.setVolume(0);
    return success;
}

//--------------------------------------------------------------
void Layer::bind() {
    if (!valid_)
        return;
    player_.getTexture().bind(id_);
}

//--------------------------------------------------------------
void Layer::unbind()
{
    if (!valid_)
        return;
    player_.getTexture().unbind(id_);
}

//--------------------------------------------------------------
void Layer::play()
{
    Playable::play();
    player_.setPaused(!isPlaying());
}

//--------------------------------------------------------------
void Layer::pause()
{
    Playable::pause();
    if (retrigger_)
        player_.setFrame(0);
    player_.setPaused(true);
}

//--------------------------------------------------------------
void Layer::playPause()
{
    Playable::playPause();
    if (!isPlaying() && retrigger_)
        player_.setFrame(0);
    player_.setPaused(!isPlaying());
}

//--------------------------------------------------------------
bool Layer::isFrameNew()
{
    return player_.isFrameNew();
}

//--------------------------------------------------------------
void Layer::update()
{
    player_.update();
}

} // namespace skinny
