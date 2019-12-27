#pragma once
#include <cstdint>

namespace skinny {

typedef uint8_t midiNote;

static const int MAX_LAYERS = 8;
static const int MAX_EFFECTS = 5;
static const float MAX_7BITF = 127.0f;
static constexpr const char * VERSION = "0.6.2-alpha";
static const midiNote DEFAULT_MASTER_ALPHA_CONTROL = 16;

// #TODO Create macros for enum strings

enum class BlendMode {
    Invalid     = -1,
    Normal      = 0, // s
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

enum class EffectType {
    Invalid     = -1,
    Solarize    = 0,
    Posterize   = 1,
    ColorShift  = 2,
    Overdrive   = 3,
    Count       = 4
};

const char* c_str(EffectType type);

} // namespace skinny
