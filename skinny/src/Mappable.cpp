#include "Mappable.h"
#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
Mappable::Mappable(midiNote note, midiNote cc) :
    note_(note),
    cc_(cc)
{}

//--------------------------------------------------------------
void Mappable::setup()
{
  ofAddListener(getStatus().midi->noteOnEvent, this, &Mappable::onNoteOn);
  ofAddListener(getStatus().midi->noteOffEvent, this, &Mappable::onNoteOff);
  ofAddListener(getStatus().midi->controlChangeEvent, this, &Mappable::onControlChange);
}

//--------------------------------------------------------------
void Mappable::exit()
{
  ofRemoveListener(getStatus().midi->noteOnEvent, this, &Mappable::onNoteOn);
  ofRemoveListener(getStatus().midi->noteOffEvent, this, &Mappable::onNoteOff);
  ofRemoveListener(getStatus().midi->controlChangeEvent, this, &Mappable::onControlChange);
}

//--------------------------------------------------------------
bool Mappable::isCorrectChannel(int channel)
{
  return channel == getStatus().showDescription->getMidiChannel();
}

} // namespace skinny
