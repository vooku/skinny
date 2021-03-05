#include "base.h"

namespace skinny {

//--------------------------------------------------------------
const char * c_str(BlendMode blendMode)
{
    switch (blendMode)
    {
    case BlendMode::Overlay:
        return "Overlay";
    case BlendMode::Multiply:
        return "Multiply";
    case BlendMode::Screen:
        return "Screen";
    case BlendMode::Darken:
        return "Darken";
    case BlendMode::Lighten:
        return "Lighten";
    case BlendMode::LinearDodge:
        return "LinearDodge";
    case BlendMode::Difference:
        return "Difference";
    case BlendMode::Exclusion:
        return "Exclusion";
    default:
        return "Invalid";
    }
}

//--------------------------------------------------------------
const char* c_str(EffectType type) {
    switch (type)
    {
    case EffectType::Solarize:
        return "Solarize";
    case EffectType::Posterize:
        return "Posterize";
    case EffectType::ColorShift:
        return "Color Shift";
    case EffectType::Overdrive:
        return "Overdrive";
    default:
        return "Invalid";
    }
}

} // namespace skinny
