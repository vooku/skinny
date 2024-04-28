#include "VideoLayer.h"

namespace skinny {

//--------------------------------------------------------------
VideoLayer::VideoLayer(int id, const std::filesystem::path& path, midiNote note, midiNote control) :
    Layer(id, path, note, control)
{
	  reload(path);
    valid_ = true; // the current implementation of ofMediaFoundationPlayer has no error handling
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
    player_.loadAsync(path.string());
    player_.setVolume(0);
    player_.setLoopState(OF_LOOP_NORMAL);
    return true; // useless since loading async
}

//--------------------------------------------------------------
bool VideoLayer::isLoaded() const
{
  return player_.isLoaded();
}

//--------------------------------------------------------------
void VideoLayer::bind() {
  // if using text then planes must be nonzero size
  if (player_.isLoaded() && (!player_.isUsingTexture() || player_.getTexturePlanes().size() > 0))
    player_.getTexture().bind(id_);
}

//--------------------------------------------------------------
void VideoLayer::unbind()
{
  // if using text then planes must be nonzero size
  if (player_.isLoaded() && (!player_.isUsingTexture() || player_.getTexturePlanes().size() > 0))
    player_.getTexture().unbind(id_);
}

//--------------------------------------------------------------
void VideoLayer::play()
{
  if (player_.isLoaded())
  {
    Playable::play();
    player_.setPaused(!isPlaying());
  }
}

//--------------------------------------------------------------
void VideoLayer::pause()
{
    Playable::pause();

    if (player_.isLoaded())
    {
      if (retrigger_)
        player_.setFrame(0);
      player_.setPaused(true);
    }
}

//--------------------------------------------------------------
void VideoLayer::playPause()
{
    Playable::playPause();

    if (player_.isLoaded())
    {
      if (!isPlaying() && retrigger_)
        player_.setFrame(0);
      player_.setPaused(!isPlaying());
    }
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
