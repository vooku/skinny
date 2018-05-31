#pragma once

#include "OfxMidi.h"
#include "meta.h"

class Scene {
public:
    static const int maxLayers = 5;

    Scene(const SceneDescription& description);

    bool isFrameNew();
    void newMidiMessage(ofxMidiMessage & msg);
    void playPauseLayer(int idx);

    void setupUniforms(ofShader& shader) const;

private:
    struct Uniforms {
        int nLayers;
        int active[maxLayers];
        glm::ivec2 dimensions[maxLayers];
        int blendingModes[maxLayers];
    } mutable uniforms;

    std::vector<std::unique_ptr<Layer>> layers_;

};

