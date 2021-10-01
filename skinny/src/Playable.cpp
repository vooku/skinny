#include "Playable.h"

namespace skinny {

//--------------------------------------------------------------
Playable::Playable(midiNote note, midiNote cc)
	: Mappable(note, cc)
{
}

//--------------------------------------------------------------
void Playable::onNoteOn(NoteMessage& msg)
{
	if (isCorrectChannel(msg.channel_) && msg.note_ == note_)
		play();
}

//--------------------------------------------------------------
void Playable::onNoteOff(NoteMessage& msg)
{
	if (isCorrectChannel(msg.channel_) && msg.note_ == note_)
		pause();
}

//--------------------------------------------------------------
void Playable::onControlChange(ControlChangeMessage& msg)
{
	if (isCorrectChannel(msg.channel_) && msg.control_ == cc_)
		ccValue_ = msg.value_;
}

//--------------------------------------------------------------
void Playable::play()
{
	if (!mute_)
		playing_ = true;
}

//--------------------------------------------------------------
void Playable::pause()
{
	playing_ = false;
}

//--------------------------------------------------------------
void Playable::playPause()
{
	if (playing_)
		playing_ = false;
	else if (!mute_)
		playing_ = true;
}

//--------------------------------------------------------------
bool Playable::isPlaying() const
{
	return playing_;
}

//--------------------------------------------------------------
void Playable::setMute(bool mute)
{
	mute_ = mute;
	playing_ = playing_ && !mute_;
}

}
