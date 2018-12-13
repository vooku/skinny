#pragma once

#include "Mappable.h"

// TODO Using the enum as a key in a map seems like bad design.
class Effect : public Mappable {
public:
    enum class Type {
        Invalid       = -1,
        Inverse       =  0,
        ReducePalette =  1,
        ColorShift    =  2,
        ColorShift2   =  3,
        Count         =  4
    };

    static char* c_str(Type);

    Effect() : Mappable() { }
    explicit Effect(midiNote note) : Mappable(note) { }

};