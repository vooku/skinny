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

    static bool isCorrectChannel(int channel);
    virtual void onNoteOn(NoteMessage& msg);
    virtual void onNoteOff(NoteMessage& msg);
    virtual void onControlChange(ControlChangeMessage& msg);

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
