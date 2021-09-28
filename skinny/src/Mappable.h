#pragma once

#include "base.h"

namespace skinny {

class Mappable {
public:
    Mappable() = default;
    Mappable(midiNote note, midiNote cc);
    virtual ~Mappable() = default;

    virtual void init();
    virtual void done();

    virtual void play();
    virtual void pause();
    virtual void playPause();
    virtual bool isPlaying() const { return active_; }

    virtual void onNoteOn(midiNote& note);
    virtual void onNoteOff(midiNote& note);
    virtual void onControlChange(ControlChange& change);

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
    int ccValue_ = 0;

};

} // namespace skinny
