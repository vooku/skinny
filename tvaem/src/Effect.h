#pragma once

#include "Mappable.h"

namespace skinny {

class Effect : public Mappable {
    friend class Gui;

public:
    explicit Effect(int id);
    Effect(int id, EffectType type, midiNote note, midiNote control, int param);

    int getParam() const;
    void setParam(int param);

    const EffectType type;

private:
    const int id_;
    int param_ = 127;

};

} // namespace skinny
