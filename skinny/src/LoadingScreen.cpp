#include "LoadingScreen.h"

namespace skinny {

//--------------------------------------------------------------
bool LoadingScreen::reload(const ShowDescription& description)
{
	note_ = description.getLoadingScreensNote();
	cc_ = description.getLoadingScreensCc();

	// no clear for queue lmao
	T_Images empty;
	std::swap(images_, empty);

	const auto& path = description.getLoadingScreensPath();
	if (path.empty())
	{
		return true;
	}

	if (path.has_extension()) // has_filename is useless
	{
		return loadImg(path);
	}
	else
	{
		for (const auto& file : std::filesystem::directory_iterator{ path })
		{
			loadImg(file);
		}
	}

	return true;
}

//--------------------------------------------------------------
bool LoadingScreen::loadImg(const std::filesystem::path& path)
{
	auto newImg = std::make_shared<ofImage>();
	const auto loaded = newImg->load(path);
	if (loaded)
	{
		images_.emplace(newImg);
	}
	else
	{
		ofLog(OF_LOG_ERROR, "Cannot load %s at %s",
					path.filename().string().c_str(), path.parent_path().string().c_str());
	}

	return loaded;
}

//--------------------------------------------------------------
void LoadingScreen::advance()
{
	if (images_.size() > 1)
	{
		images_.push(images_.front());
		images_.pop();
	}
}

//--------------------------------------------------------------
const ofTexture& LoadingScreen::getLoadingScreen() const
{
	if (!images_.empty())
	{
		return images_.front()->getTexture();
	}

	static ofTexture invalid;
	return invalid;
}

//--------------------------------------------------------------
float LoadingScreen::getAlpha() const
{
	return clamp(ccValue_, 0, MAX_7BIT) / MAX_7BITF;
}

//--------------------------------------------------------------
void LoadingScreen::onNoteOn(NoteMessage& msg)
{
	if (isCorrectChannel(msg.channel_) && msg.note_ == note_)
		advance();
}

//--------------------------------------------------------------
void LoadingScreen::onNoteOff(NoteMessage& msg)
{
	// nothing
}

}