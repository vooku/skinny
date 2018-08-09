#pragma once

#include <set>
#include "base.h"

class Mappable {
public:
    typedef std::set<midiNote> MidiMap;

    Mappable() : active_(false), mute_(false) { }
    Mappable(const MidiMap& map) : active_(false), mute_(false), midiMap_(map) { }
    virtual ~Mappable() { }

    virtual void play() { active_ = true && !mute_; }
    virtual void pause() { active_ = false; }
    virtual void playPause() { active_ = !active_ && !mute_; }
    virtual bool isPlaying() const { return active_; }

    virtual void addMidiNote(int note) { midiMap_.insert(note); }
    virtual void removeMidiNote(int note) { midiMap_.erase(note); }
    virtual void clearMidiNotes() { midiMap_.clear(); }
    virtual bool containsMidiNote(int note) const { return midiMap_.count(note); }
    virtual void replaceMidiMap(const MidiMap& newMap) { midiMap_ = { newMap }; }
    virtual const MidiMap& getMapping() const { return midiMap_; }

    virtual void setMapping(const MidiMap& newMap) { midiMap_ = newMap; }
    void setMute(bool mute) { mute_ = mute; }

protected:
    bool active_, mute_; 
    MidiMap midiMap_;

};
