#pragma once

#include "Mappable.h"

class Effect : public Mappable {
public:
    enum class Type {
        Invalid       = -1,
        Inverse       =  0,
        ReducePalette =  1,
        ColorShift    =  2,
        ColorShift2   =  3,
        Count         =  4
    } const type;

    static const int MIDI_OFFSET = 48;

    static char* c_str(Type);

    Effect();
    Effect(Type type, midiNote note);
};