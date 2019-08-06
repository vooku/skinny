#pragma once
#include "Scene.h"

class Gui;

typedef std::shared_ptr<Scene> ScenePtr;

class Show
{
public:
    typedef std::array<std::unique_ptr<Effect>, MAX_EFFECTS> Effects;

    friend class Gui;

    Show(int width, int height);

    void draw();
    Scene::FoundMappables newMidiMessage(ofxMidiMessage & msg);
    bool reload(const ShowDescription& description);
    void playPauseEffect(int i);

    ScenePtr getCurrentScene() const;
    const Effects& getEffects() const;
    midiNote getAlphaControl() const;
    float getAlpha() const;

    void setAlphaControl(const midiNote& control);

private:
    void setupUniforms() const;
    bool hasActiveFX() const;

    mutable ofShader shader_;
    const int width_, height_;
    ScenePtr currentScene_;

    float masterAlpha_ = 1.0f;
    midiNote masterAlphaControl_ = DEFAULT_MASTER_ALPHA_CONTROL;

    Effects effects_;
    mutable std::map<Effect::Type, bool> effectUniforms_;

};