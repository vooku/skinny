#pragma once

#include "Mappable.h"

namespace skinny {

//--------------------------------------------------------------
class Playable : public Mappable {
public:
	Playable() = default;
	Playable(midiNote note, midiNote cc);

	virtual void onNoteOn(NoteMessage& msg) override;
	virtual void onNoteOff(NoteMessage& msg) override;
	virtual void onControlChange(ControlChangeMessage& msg) override;

	virtual void play();
	virtual void pause();
	virtual void playPause();
	bool isPlaying() const;
	void setMute(bool mute);

private:
	bool playing_ = false;
	bool mute_ = false;

};

}
