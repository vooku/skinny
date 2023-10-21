#pragma once

#include "Playable.h"
#include "Meta.h"

#include "ofImage.h"

#include <queue>
#include <filesystem>

namespace skinny {

class LoadingScreen : public Playable {
public:
	bool	reload(const ShowDescription& description);
	void	advance();

	const ofTexture&	getLoadingScreen() const;
	float							getAlpha() const;

	virtual void onNoteOn(NoteMessage& msg) override;
	virtual void onNoteOff(NoteMessage& msg) override;

private:
	bool	loadImg(const std::filesystem::path& path);

	using T_Images = std::queue<std::shared_ptr<ofImage>>;
	T_Images images_;

};

}