#include "VideoLayer.h"

namespace skinny {

//--------------------------------------------------------------
VideoLayer::VideoLayer(int id, const std::filesystem::path& path, midiNote note, midiNote control) :
    Layer(id, path, note, control)
{
	  valid_ = reload(path);

		if (valid_) {
			  ofLog(OF_LOG_VERBOSE, "Loaded %s.", name_.c_str());
        player_.setLoopState(OF_LOOP_NORMAL);
    }
    else {
        ofLog(OF_LOG_ERROR, "Cannot load %s at %s", name_.c_str(), path.c_str());
    }
}

//--------------------------------------------------------------
VideoLayer::VideoLayer(int id, ErrorType error) :
    Layer(id, error)
{
}

//--------------------------------------------------------------
VideoLayer::~VideoLayer()
{
    player_.closeMovie();
}

//--------------------------------------------------------------
bool VideoLayer::reload(const std::filesystem::path& path)
{
    player_.closeMovie();
    const auto success = player_.load(path.string());
    player_.setVolume(0);
    return success;
}

//--------------------------------------------------------------
void VideoLayer::bind() {
    if (!valid_)
        return;
    player_.getTexture().bind(id_);
}

//--------------------------------------------------------------
void VideoLayer::unbind()
{
    if (!valid_)
        return;
    player_.getTexture().unbind(id_);
}

//--------------------------------------------------------------
void VideoLayer::play()
{
    Playable::play();
    player_.setPaused(!isPlaying());
}

//--------------------------------------------------------------
void VideoLayer::pause()
{
    Playable::pause();
    if (retrigger_)
        player_.setFrame(0);
    player_.setPaused(true);
}

//--------------------------------------------------------------
void VideoLayer::playPause()
{
    Playable::playPause();
    if (!isPlaying() && retrigger_)
        player_.setFrame(0);
    player_.setPaused(!isPlaying());
}

//--------------------------------------------------------------
bool VideoLayer::isFrameNew()
{
    return player_.isFrameNew();
}

//--------------------------------------------------------------
void VideoLayer::update()
{
    player_.update();
}

} // namespace skinny
