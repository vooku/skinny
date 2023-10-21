#pragma once

#include "Playable.h"
#include "Meta.h"

#include "ofImage.h"

namespace skinny {

class LoadingScreen : public Playable {
public:
	bool	reload(const ShowDescription& description);

	const ofTexture&	getNext() const;

private:
	// #TODO more img
	// ##TODO timing

	ofImage img_;

};

}