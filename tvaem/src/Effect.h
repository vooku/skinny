#pragma once

#include "OfxMidi.h"
#include "Mappable.h"
#include <string>

// TODO This class seems to be poorly designed.
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
    Effect(const MidiMap& map) : Mappable(map) { }
    
private:

};