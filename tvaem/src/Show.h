#pragma once
#include "Scene.h"

class Show
{
public:
    typedef std::unordered_map<Effect::Type, Effect> Effects;

    Show(int width, int height);

    void update();
    void draw();
    Scene::FoundMappables newMidiMessage(ofxMidiMessage & msg);
    bool reload(const SceneDescription& description);

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