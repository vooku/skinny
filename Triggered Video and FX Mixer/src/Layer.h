#pragma once

#include "ofMain.h"
#include <set>

class Layer {
public:
    typedef std::set<uint8_t> MidiMap;

    enum class BlendMode {
        Invalid = -1,
        Normal = 0,      // s
        Multiply = 1,    // b * s
        Screen = 2,      // 1 - ((1 - b) * (1 - s))
        Darken = 3,      // min(b, s)
        Lighten = 4,     // max(b, s)
        LinearDodge = 5, // s + b
        Difference = 6,  // |b - s|
        Exclusion = 7    // b + s + 2 * b * s
    };

    Layer(int id);
    Layer(int id, const std::string& filename);
    ~Layer();

    bool reload(const std::string& filename);
    void play();
    void pause();
    void playPause();
    bool isFrameNew();

    void addMidiNote(int note) { midiMap_.insert(note); }
    void removeMidiNote(int note) { midiMap_.erase(note); }
    void clearMidiNotes() { midiMap_.clear(); }
    bool containsMidiNote(int note) const { return midiMap_.count(note); }
    void replaceMidiMap(const MidiMap& newMap) { midiMap_ = { newMap }; }
    
    float getWidth() const { return player_.getWidth(); }
    float getHeight() const { return player_.getHeight(); }
    float getAlpha() const { return globalAlpha_; }
    BlendMode getBlendMode() const { return blendMode_; }

    void setBlendMode(BlendMode newMode) { blendMode_ = newMode; }

private:
    ofVideoPlayer player_;
    bool paused_;
    const int id_;
    float globalAlpha_;
    BlendMode blendMode_;
    MidiMap midiMap_;
};
