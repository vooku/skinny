#include "Effect.h"

namespace skinny {

Effect::Effect(int id) :
    Mappable(MIDI_OFFSET + id, ALPHA_MIDI_OFFSET + id),
    id_(id),
    type(static_cast<EffectType>(id))
{}

Effect::Effect(int id, EffectType type, midiNote note, midiNote control, int param) :
    Mappable(note == -1 ? MIDI_OFFSET + id : note,
             control == -1 ? ALPHA_MIDI_OFFSET + id : note),
    id_(id),
    type(type),
    param_(param)
{}


int Effect::getParam() const
{
    return param_;
}

void Effect::setParam(int param)
{
    param_ = param;
}

} // namespace skinny
