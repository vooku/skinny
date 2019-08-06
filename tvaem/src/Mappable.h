#pragma once

#include "base.h"

class Mappable {
public:
    Mappable() = default;
    Mappable(midiNote note, midiNote cc);
    virtual ~Mappable() = default;

    virtual void play() { active_ = !mute_; }
    virtual void pause() { active_ = false; }
    virtual void playPause() { active_ = !active_ && !mute_; }
    virtual bool isPlaying() const { return active_; }

    // TODO virtual bool newMidiMessage(ofxMidiMessage& msg);

    virtual midiNote getNote() const { return note_; }
    virtual midiNote getCc() const { return cc_; }

    virtual void setNote(midiNote note) { note_ = note; }
    virtual void setCc(midiNote cc) { cc_ = cc; }
    void setMute(bool mute);

protected:
    bool active_ = false;
    bool mute_ = false;
    midiNote note_ = 0;
    midiNote cc_ = 0;

};
