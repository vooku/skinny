#pragma once

#include "OfxMidi.h"
#include "meta.h"
#include <unordered_map>

class Scene {
public:
    static const int maxLayers = 7;

    class LayerNames {
    public:
        LayerNames(const SceneDescription& description);

        auto size() const { return names.size(); }
        const std::string& operator[] (int i) const { return names[i]; }

    private:
        std::vector<std::string> names;

    } const layerNames;

    class EffectNames {
    public:
        EffectNames(const SceneDescription& description);

        auto size() const { return names.size(); }
        const std::string& operator[] (int i) const { return names[i]; }

    private:
        std::vector<std::string> names;

    } const effectNames;

    Scene(const SceneDescription& description);

    void bindTextures();
    bool isFrameNew();
    bool hasActiveFX() const;
    void newMidiMessage(ofxMidiMessage & msg);
    void playPauseLayer(int idx);

    void setupUniforms(ofShader& shader) const;

    const std::string& getName() const { return name_; }
    bool isValid() { return valid_; }

private:
    struct Uniforms {
        int nLayers;
        int playing[maxLayers];
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
    bool valid_;

};
