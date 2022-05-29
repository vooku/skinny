#include "ImageLayer.h"

namespace skinny {

//--------------------------------------------------------------
ImageLayer::ImageLayer(int id, const std::filesystem::path& path, midiNote note, midiNote control) :
	Layer(id, path, note, control)
{
	valid_ = reload(path);

	if (valid_) {
		ofLog(OF_LOG_VERBOSE, "Loaded %s.", name_.c_str());
	}
	else {
		ofLog(OF_LOG_ERROR, "Cannot load %s at %s", name_.c_str(), path.c_str());
	}
}

//--------------------------------------------------------------
ImageLayer::ImageLayer(int id, ErrorType error) :
	Layer(id, error)
{
}

//--------------------------------------------------------------
bool ImageLayer::reload(const std::filesystem::path& path)
{
	const auto success = image_.load(path.string());
	return success;
}

//--------------------------------------------------------------
void ImageLayer::bind() {
	if (!valid_)
		return;
	image_.getTexture().bind(id_);
}

//--------------------------------------------------------------
void ImageLayer::unbind()
{
	if (!valid_)
		return;
	image_.getTexture().unbind(id_);
}

//--------------------------------------------------------------
bool ImageLayer::isFrameNew()
{
	return true;
}

} // namespace skinny
