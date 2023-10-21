#include "LoadingScreen.h"

namespace skinny {

//--------------------------------------------------------------
bool LoadingScreen::reload(const ShowDescription& description)
{
	note_ = description.getLoadingScreensNote();

	img_.clear();

	const auto& path = description.getLoadingScreensPath();
	if (!path.empty())
	{
		return img_.load(path);
	}

	return true;
}

//--------------------------------------------------------------
const ofTexture& LoadingScreen::getNext() const
{
	return img_.getTexture();
}

}