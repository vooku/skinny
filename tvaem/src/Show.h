#pragma once
#include "Scene.h"

class Show
{
public:
    Show(ofShader& shader, int width, int height);

    void update();
    Scene::FoundMappables newMidiMessage(ofxMidiMessage & msg);
    bool reload(const SceneDescription& description);

    auto getCurrentScene() const { return currentScene_; }

private:
    ofShader& shader_;
    const int width_, height_;
    std::shared_ptr<Scene> currentScene_;
};