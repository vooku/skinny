#pragma once

#include "base.h"

namespace skinny {

//--------------------------------------------------------------
class Mappable {
public:
    Mappable() = default;
    Mappable(midiNote note, midiNote cc);
    virtual ~Mappable() = default;

    static bool isCorrectChannel(int channel);

    virtual void init();
    virtual void done();

    // dummy implementation so there's no need to implement everything
    virtual void onNoteOn(NoteMessage& msg) {};
    virtual void onNoteOff(NoteMessage& msg) {};
    virtual void onControlChange(ControlChangeMessage& msg) {};

    virtual midiNote getNote() const { return note_; }
    virtual midiNote getCc() const { return cc_; }

    void setNote(midiNote note) { note_ = note; }
    void setCc(midiNote cc) { cc_ = cc; }

protected:
    midiNote note_ = 0;
    midiNote cc_ = 0;
    int ccValue_ = 0;

};

} // namespace skinny
