#include "Effect.h"

namespace skinny {

//--------------------------------------------------------------
Effect::Effect(int id) :
    Playable(EFFECT_NOTE_OFFSET + id, EFFECT_CC_OFFSET + id),
    id_(id),
    type(static_cast<EffectType>(id))
{}

//--------------------------------------------------------------
Effect::Effect(int id, EffectType type, midiNote note, midiNote control, int param) :
    Playable(note == -1 ? EFFECT_NOTE_OFFSET + id : note,
             control == -1 ? EFFECT_CC_OFFSET + id : control),
    id_(id),
    type(type)
{
    ccValue_ = param;
}

//--------------------------------------------------------------
int Effect::getParam() const
{
    return ccValue_;
}

//--------------------------------------------------------------
void Effect::setParam(int param)
{
  ccValue_ = param;
}

} // namespace skinny
