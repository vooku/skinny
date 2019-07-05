#pragma once
#include "Scene.h"

class Show
{
public:
    Show(ofShader& shader, int width, int height);

    void update();
    void newMidiMessage(ofxMidiMessage & msg);
    bool reload(const SceneDescription& description);



private:
    ofShader& shader_;
    const int width_, height_;
    std::unique_ptr<Scene> currentScene_;
};