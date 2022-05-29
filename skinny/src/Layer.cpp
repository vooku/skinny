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
	ccValue_ = MAX_7BIT;
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

}
