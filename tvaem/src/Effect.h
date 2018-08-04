#pragma once

#include "OfxMidi.h"
#include "Mappable.h"
#include <string>

class Effect : public Mappable {
public:
    enum class Type {
        Invalid = -1,
        Inverse = 0,
        ReducePalette = 1,
        ColorShift = 2,
        ColorShift2 = 3
    };

    Effect() : Mappable() { }
    Effect(const MidiMap& map) : Mappable(map) { }
    
private:

};