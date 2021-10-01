#pragma once

#include "Playable.h"

namespace skinny {

class Effect : public Playable {
    friend class Gui;

public:
    explicit Effect(int id);
    Effect(int id, EffectType type, midiNote note, midiNote control, int param);

    int getParam() const;

    const EffectType type;

private:
    const int id_;

};

} // namespace skinny
