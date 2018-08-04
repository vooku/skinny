#include "Effect.h"

char* Effect::c_str(Type type) {
    switch (type)
    {    case Effect::Type::Inverse:
        return "Inverse";
    case Effect::Type::ReducePalette:
        return "Reduce Palette";
    case Effect::Type::ColorShift:
        return "Color Shift";
    case Effect::Type::ColorShift2:
        return "Double Shift";
    default:
        return "Invalid";
    }
}