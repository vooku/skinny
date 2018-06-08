#pragma once

#include "OfxMidi.h"
#include "meta.h"
#include <unordered_map>

class Scene {
public:
    static const int maxLayers = 7;

    Scene(const SceneDescription& description);

    void bindTextures();
    bool isFrameNew();
    bool hasActiveFX() const;
    void newMidiMessage(ofxMidiMessage & msg);
    void playPauseLayer(int idx);

    void setupUniforms(ofShader& shader) const;

    const std::string& getName() const { return name_; }

private:
    struct Uniforms {
        int nLayers;
        int active[maxLayers];
        glm::ivec2 dimensions[maxLayers];
        int blendingModes[maxLayers];
        bool inverse;
        bool reducePalette;
        bool colorShift;
        bool colorShift2;
    } mutable uniforms_;

    const std::string name_;
    std::vector<std::unique_ptr<Layer>> layers_;
    std::unordered_map<Effect::Type, Effect> effects_;

};

