#include "base.h"

namespace skinny {

//--------------------------------------------------------------
const char * c_str(BlendMode blendMode)
{
    static_assert(static_cast<int>(BlendMode::Count) == 8, "Do not forget to add new values here.");
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
    static_assert(static_cast<int>(EffectType::Count) == 7, "Do not forget to add new values here.");

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
    case EffectType::HOffset:
        return "Horizontal Offset";
    case EffectType::VOffset:
        return "Vertical Offset";
    case EffectType::Desaturate:
        return "Desaturate";
    default:
        return "Invalid";
    }
}

} // namespace skinny
