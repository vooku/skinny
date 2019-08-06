#include "Effect.h"

char* Effect::c_str(Type type) {
    switch (type)
    {    case Type::Inverse:
        return "Inverse";
    case Type::ReducePalette:
        return "Reduce Palette";
    case Type::ColorShift:
        return "Color Shift";
    case Type::ColorShift2:
        return "Double Shift";
    default:
        return "Invalid";
    }
}

Effect::Effect() :
    Mappable(),
    type(Type::Inverse)
{}

Effect::Effect(Type type, midiNote note, midiNote control, int param) :
    Mappable(note, control),
    type(type),
    param_(param)
{}
