#include "Effect.h"

namespace skinny {

char* Effect::c_str(Type type) {
    switch (type)
    {
    case Type::Solarize:
        return "Solarize";
    case Type::Posterize:
        return "Posterize";
    case Type::ColorShift:
        return "Color Shift";
    case Type::Overdrive:
        return "Overdrive";
    default:
        return "Invalid";
    }
}

Effect::Effect(int id) :
    Mappable(MIDI_OFFSET + id, ALPHA_MIDI_OFFSET + id),
    id_(id),
    type(static_cast<Type>(id))
{}

Effect::Effect(int id, Type type, midiNote note, midiNote control, int param) :
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
