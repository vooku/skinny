#include "Mappable.h"

Mappable::Mappable() :
    active_(false),
    mute_(false)
{
}

Mappable::Mappable(const MidiMap & map) :
    active_(false),
    mute_(false),
    midiMap_(map)
{
}

void Mappable::setMute(bool mute)
{
    mute_ = mute;
    active_ = active_ && !mute_;
}
