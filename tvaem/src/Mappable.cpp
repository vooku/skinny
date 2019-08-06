#include "Mappable.h"


Mappable::Mappable(midiNote note, midiNote cc) :
    note_(note),
    cc_(cc)
{}

void Mappable::setMute(bool mute)
{
    mute_ = mute;
    active_ = active_ && !mute_;
}
