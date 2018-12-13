#include "Mappable.h"

void Mappable::setMute(bool mute)
{
    mute_ = mute;
    active_ = active_ && !mute_;
}
