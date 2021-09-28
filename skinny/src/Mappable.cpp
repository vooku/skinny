#include "Mappable.h"
#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
Mappable::Mappable(midiNote note, midiNote cc) :
    note_(note),
    cc_(cc)
{}

//--------------------------------------------------------------
void Mappable::init()
{
  ofAddListener(getStatus().midi->noteOnEvent, this, &Mappable::onNoteOn);
  ofAddListener(getStatus().midi->noteOffEvent, this, &Mappable::onNoteOff);
  ofAddListener(getStatus().midi->controlChangeEvent, this, &Mappable::onControlChange);
}

//--------------------------------------------------------------
void Mappable::done()
{
  ofRemoveListener(getStatus().midi->noteOnEvent, this, &Mappable::onNoteOn);
  ofRemoveListener(getStatus().midi->noteOffEvent, this, &Mappable::onNoteOff);
  ofRemoveListener(getStatus().midi->controlChangeEvent, this, &Mappable::onControlChange);
}

//--------------------------------------------------------------
void Mappable::play()
{
  if (!mute_)
    active_ = true;
}

//--------------------------------------------------------------
void Mappable::pause()
{
  active_ = false;
}

//--------------------------------------------------------------
void Mappable::playPause()
{
  if (active_)
    active_ = false;
  else if (!mute_)
    active_ = true;
}

//--------------------------------------------------------------
void Mappable::onNoteOn(midiNote& note)
{
  if (note == note_)
    play();
}

//--------------------------------------------------------------
void Mappable::onNoteOff(midiNote& note)
{
  if (note == note_)
    pause();
}

//--------------------------------------------------------------
void Mappable::onControlChange(ControlChange& change)
{
  if (change.control == cc_)
    ccValue_ = change.value;
}

//--------------------------------------------------------------
void Mappable::setMute(bool mute)
{
    mute_ = mute;
    active_ = active_ && !mute_;
}

} // namespace skinny
