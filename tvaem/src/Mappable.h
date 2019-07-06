#pragma once

#include "base.h"

class Mappable {
public:
    Mappable() = default;
    explicit Mappable(midiNote note) : note_(note) { }
    virtual ~Mappable() { }

    virtual void play() { active_ = !mute_; }
    virtual void pause() { active_ = false; }
    virtual void playPause() { active_ = !active_ && !mute_; }
    virtual bool isPlaying() const { return active_; }

    // TODO virtual bool newMidiMessage(ofxMidiMessage& msg);

    virtual midiNote getNote() const { return note_; }

    virtual void setNote(midiNote note) { note_ = note; }
    void setMute(bool mute);

protected:
    bool active_ = false;
    bool mute_ = false;
    midiNote note_ = 0;

};
