#pragma once
#include "Scene.h"

class Gui;

class Show
{
public:
    typedef std::unordered_map<Effect::Type, Effect> Effects;

    friend class Gui;

    Show(int width, int height);

    void update();
    void draw();
    Scene::FoundMappables newMidiMessage(ofxMidiMessage & msg);
    bool reload(const SceneDescription& description);
    void playPauseEffect(Effect::Type type);

    auto getCurrentScene() const { return currentScene_; }
    const auto& getEffects() const { return effects_; }

private:
    struct Uniforms {
        bool inverse = false;
        bool reducePalette = false;
        bool colorShift = false;
        bool colorShift2 = false;
    } mutable uniforms_;

    void setupUniforms() const;
    bool hasActiveFX() const;

    mutable ofShader shader_;
    ofTexture dst_;
    const int width_, height_;
    std::shared_ptr<Scene> currentScene_;

    Effects effects_;

};