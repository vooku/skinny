#pragma once
#include <cstdint>

typedef uint8_t midiNote;

static const int MAX_LAYERS = 7;
static const int MAX_EFFECTS = 4;
static constexpr const char * VERSION = "0.5.1-alpha";
static const midiNote DEFAULT_MASTER_ALPHA_CONTROL = 8;