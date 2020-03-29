#pragma once
#include <cstdint>

namespace skinny {

typedef uint8_t midiNote;

static const int MAX_LAYERS = 8;
static const int MAX_EFFECTS = 5;
static const float MAX_7BITF = 127.0f;
static constexpr const char* VERSION = "0.7.2-alpha";
static constexpr const char* NAME = "Skinny Mixer";
static constexpr const char* AUTHOR = "Vadim Vooku Petrov";
static const midiNote DEFAULT_MASTER_ALPHA_CONTROL = 16;
static const midiNote LAYER_NOTE_OFFSET = 0;
static const midiNote LAYER_CC_OFFSET = 0;
static const midiNote EFFECT_NOTE_OFFSET = 8;
static const midiNote EFFECT_CC_OFFSET = 8;
static const int MAX_LABEL_LENGTH = 34;
static constexpr const char* FONT_REGULAR = "fonts/IBMPlexMono-Regular.ttf";
static constexpr const char* FONT_ITALIC = "fonts/IBMPlexSerif-Italic.ttf";
static constexpr const char* DEFAULT_FILENAME = "config.xml";

// #TODO Create macros for enum strings

enum class BlendMode {
    Invalid     = -1,
    Overlay      = 0, // s
    Multiply    = 1, // b * s
    Screen      = 2, // 1 - ((1 - b) * (1 - s))
    Darken      = 3, // min(b, s)
    Lighten     = 4, // max(b, s)
    LinearDodge = 5, // s + b
    Difference  = 6, // |b - s|
    Exclusion   = 7, // b + s - 2 * b * s
    Count       = 8  // Used for iteration
};

const char* c_str(BlendMode blendMode);

static const BlendMode DEFAULT_BLEND_MODE = BlendMode::LinearDodge;

enum class EffectType {
    Invalid     = -1,
    Solarize    = 0,
    Posterize   = 1,
    ColorShift  = 2,
    Overdrive   = 3,
    Count       = 4
};

const char* c_str(EffectType type);

enum class LoadDir {
    Current, Forward, Backward, Jump, None
};

} // namespace skinny
