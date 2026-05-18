#pragma once

#include <filesystem>
#include <queue>

#include "Meta.h"
#include "Playable.h"
#include "ofImage.h"

namespace skinny {

class LoadingScreen : public Playable
{
public:
	bool reload(const ShowDescription& description);
	void advance();

	const ofTexture& getLoadingScreen() const;
	float getAlpha() const;

	virtual void onNoteOn(NoteMessage& msg) override;
	virtual void onNoteOff(NoteMessage& msg) override;

private:
	bool loadImg(const std::filesystem::path& path);

	using T_Images = std::queue<std::shared_ptr<ofImage>>;
	T_Images images_;
};

} // namespace skinny