#pragma once
#include "Scene.h"

class Gui;

class Show
{
public:
    typedef std::array<std::unique_ptr<Effect>, MAX_EFFECTS> Effects;

    friend class Gui;

    Show(int width, int height);

    void update();
    void draw();
    Scene::FoundMappables newMidiMessage(ofxMidiMessage & msg);
    bool reload(const ShowDescription& description);
    void playPauseEffect(int i);

    auto getCurrentScene() const { return currentScene_; }
    const auto& getEffects() const { return effects_; }

private:
    mutable std::map<Effect::Type, bool> effectUniforms_;

    void setupUniforms() const;
    bool hasActiveFX() const;

    mutable ofShader shader_;
    const int width_, height_;
    std::shared_ptr<Scene> currentScene_;

    Effects effects_;

    ofVideoPlayer player_; // #TODO remove
};