#pragma once

#include "Mappable.h"

class Effect : public Mappable {
    friend class Gui;

public:
    enum class Type {
        Invalid       = -1,
        Solarize      =  0,
        Posterize     =  1,
        ColorShift    =  2,
        Overdrive     =  3,
        Count         =  4
    } const type;

    static const int MIDI_OFFSET = 8;
    static const int ALPHA_MIDI_OFFSET = 8;

    static char* c_str(Type);

    explicit Effect(int id);
    Effect(int id, Type type, midiNote note, midiNote control, int param);

    int getParam() const;
    void setParam(int param);

private:
    const int id_;
    int param_ = 127;

};