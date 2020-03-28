#pragma once
#include "Scene.h"

namespace skinny {

class Gui;

typedef std::shared_ptr<Scene> ScenePtr;

class Show
{
public:
    typedef std::array<std::unique_ptr<Effect>, MAX_EFFECTS> Effects;

    friend class Gui;

    Show(int width, int height);

    void draw();
    ActiveMappables newMidiMessage(ofxMidiMessage & msg);
    bool reload(const ShowDescription& description);
    void playPauseEffect(int i);
    void update();

    ScenePtr getCurrentScene() const;
    const Effects& getEffects() const;
    midiNote getAlphaControl() const;
    float getAlpha() const;

    void setAlphaControl(const midiNote& control);

private:
    struct Uniforms {
        int fxTypes[MAX_EFFECTS];
        int fxPlaying[MAX_EFFECTS];
        float fxParam[MAX_EFFECTS];
        float masterAlpha_ = 1.0f;
    } mutable uniforms_;

    void setupUniforms() const;
    bool hasActiveFX() const;

    mutable ofShader shader_;
    const int width_, height_;
    ScenePtr currentScene_;

    midiNote masterAlphaControl_ = DEFAULT_MASTER_ALPHA_CONTROL;

    Effects effects_;

};

} // namespace skinny
