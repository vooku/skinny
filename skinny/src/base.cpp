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
    static_assert(static_cast<int>(EffectType::Count) == 13, "Do not forget to add new values here.");

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
        return "Offset Horizontally";
    case EffectType::VOffset:
        return "Offset Vertically";
    case EffectType::Desaturate:
        return "Desaturate";
    case EffectType::Blur:
        return "Blur";
    case EffectType::HBlur:
        return "Blur Horizontally";
    case EffectType::VBlur:
        return "Blur Vertically";
    case EffectType::Kaleidoscope:
        return "Kaleidoscope";
    case EffectType::Hue:
        return "Hue";
    case EffectType::Saturation:
        return "Saturation";
    default:
        return "Invalid";
    }
}

//--------------------------------------------------------------
MidiMessage::MidiMessage(int channel)
  : channel_(channel)
{
}

//--------------------------------------------------------------
NoteMessage::NoteMessage(int channel, midiNote note)
  : MidiMessage(channel),
    note_(note)
{
}

//--------------------------------------------------------------
ControlChangeMessage::ControlChangeMessage(int channel, midiNote control, int value)
  : MidiMessage(channel),
    control_(control),
    value_(value)
{
}

} // namespace skinny
