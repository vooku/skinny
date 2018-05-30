#pragma once

#include "ofMain.h"
#include <set>

class Layer {
public:
    Layer(int id);
    Layer(int id, const char * filename);
    ~Layer();

    bool reload(const char * filename);
    void play();
    void pause();
    void playPause();
    bool update();

    void addMidiNote(int note) { midiMap_.insert(note); }
    void removeMidiNote(int note) { midiMap_.erase(note); }
    void clearMidiNotes() { midiMap_.clear(); }
    bool containsMidiNote(int note) const { return midiMap_.count(note); }
    
    float getWidth() const { return player_.getWidth(); }
    float getHeight() const { return player_.getHeight(); }
    float getAlpha() const { return globalAlpha_; }

private:
    ofVideoPlayer player_;
    bool paused_;
    const int id_;
    float globalAlpha_;

    std::set<int> midiMap_;
};
