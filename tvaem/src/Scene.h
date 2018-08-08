#pragma once

#include "OfxMidi.h"
#include "meta.h"
#include <unordered_map>

class LayerNames {
public:
    LayerNames() = default;
    LayerNames(const SceneDescription& description);

    auto size() const { return names_.size(); }
    const std::string& operator[] (int i) const { return names_[i]; }

private:
    std::vector<std::string> names_;

};

class EffectNames {
public:
    EffectNames() = default;
    EffectNames(const SceneDescription& description);

    auto size() const { return names_.size(); }
    const std::string& operator[] (int i) const { return names_[i]; }

private:
    std::vector<std::string> names_;

};

class Scene {
public:
    void reload(const SceneDescription& description);
    void bindTextures();
    bool isFrameNew();
    bool hasActiveFX() const;

    /**
     * Return which layers / effects were (de)activated.
     */
    struct FoundMappables {
        std::unordered_map<int, bool> layers;
        std::unordered_map<Effect::Type, bool> effects;
    };
    FoundMappables newMidiMessage(ofxMidiMessage & msg);

    void playPauseLayer(int idx);
    void playPauseEffect(Effect::Type type);

    void setupUniforms(ofShader& shader) const;

    const std::string& getName() const { return name_; }
    bool isValid() { return valid_; }
    const auto getLayerNames() const { return layerNames_; }
    const auto getEffectNames() const { return effectNames_; }

private:
    struct Uniforms {
        int nLayers;
        int playing[MAX_LAYERS];
        glm::ivec2 dimensions[MAX_LAYERS];
        int blendingModes[MAX_LAYERS];
        bool inverse = false;
        bool reducePalette = false;
        bool colorShift = false;
        bool colorShift2 = false;
    } mutable uniforms_;

    std::string name_;
    std::vector<std::unique_ptr<Layer>> layers_;
    LayerNames layerNames_;
    std::unordered_map<Effect::Type, Effect> effects_;
    EffectNames effectNames_;
    bool valid_;

};
