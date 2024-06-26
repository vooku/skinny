#pragma once
#include <cstdint>

namespace skinny {

//--------------------------------------------------------------
using midiNote = int;

//--------------------------------------------------------------
static const int MAX_LAYERS = 8;
static const int MAX_EFFECTS = 12;
static const int MIDI_DEVICES_REFRESH_PERIOD = 1000000000; // this is 1 second in nanoseconds
static const int MIDI_MSG_REFRESH_PERIOD = 1000000000; // this is 1 second in nanoseconds
static const int MAX_7BIT = 127;
static const float MAX_7BITF = 127.0f;
static const int MAIN_WINDOW_WIDTH = 1920;
static const int MAIN_WINDOW_HEIGHT = 1080;
static const int GUI_WINDOW_WIDTH = 1500;
static const int GUI_WINDOW_HEIGHT = 800;
static const int GUI_MONITOR_SUBSAMPLE = 8;
static constexpr const char* VERSION = "0.11.2";
static constexpr const char* NAME = "Skinny Mixer";
// TODO TITLE VERSION + NAME
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
static constexpr const char* DEFAULT_EXTENSION = ".xml";

// #TODO Create macros for enum strings

//--------------------------------------------------------------
enum class BlendMode {
    Invalid = -1,
    Overlay,     // s
    Multiply,    // b * s
    Screen,      // 1 - ((1 - b) * (1 - s))
    Darken,      // min(b, s)
    Lighten,     // max(b, s)
    LinearDodge, // s + b
    Difference,  // |b - s|
    Exclusion,   // b + s - 2 * b * s
    Count        // Used for iteration
};

const char* c_str(BlendMode blendMode);

static const BlendMode DEFAULT_BLEND_MODE = BlendMode::LinearDodge;

//--------------------------------------------------------------
enum class EffectType {
    Invalid     = -1,
    Solarize    = 0,
    Posterize,
    ColorShift,
		Overdrive,
    HOffset,
    VOffset,
    Desaturate,
    Blur,
    HBlur,
    VBlur,
    Kaleidoscope,
    Hue,
    Saturation,
    Count,
};

const char* c_str(EffectType type);

//--------------------------------------------------------------
enum class LoadDir {
    CurrentLayers,
    CurrentEffects,
    CurrentAll,
    Forward,
    Backward,
    Jump,
    None,
};

//--------------------------------------------------------------
struct MidiMessage {
  explicit MidiMessage(int channel);

  int channel_;
};

//--------------------------------------------------------------
struct NoteMessage : public MidiMessage {
  NoteMessage(int channel, midiNote note);

  midiNote note_;
};

//--------------------------------------------------------------
struct ControlChangeMessage : public MidiMessage {
  ControlChangeMessage(int channel, midiNote control, int value);

  midiNote control_;
  int value_;
};

} // namespace skinny
