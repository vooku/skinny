#pragma once

#include "OfxMidi.h"
#include "meta.h"
#include <unordered_map>

class Gui;

class Scene {
public:
    typedef std::array<std::unique_ptr<Layer>, MAX_LAYERS> Layers;
    typedef std::unordered_map<Effect::Type, Effect> Effects;

    friend class Gui;

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
    FoundMappables newMidiMessage(const ofxMidiMessage & msg);

    void playPauseLayer(int idx);
    void playPauseEffect(Effect::Type type);

    void setupUniforms(ofShader& shader) const;

    void setAlpha(int alpha) const { uniforms_.alpha = (alpha < 0 ? 0 : alpha > 127 ? 127 : alpha) / 127.0f; }
    void setAlphaControl(midiNote control) { alphaControl_ = control; }

    const auto& getName() const { return name_; }
    const auto& getLayers() const { return layers_; }
    const auto& getEffects() const { return effects_; }
    bool isValid() const { return valid_; }
    midiNote getAlphaControl() const { return alphaControl_; }
    float getAlpha() const { return uniforms_.alpha; }

private:
    struct Uniforms {
        int nLayers;
        int playing[MAX_LAYERS];
        glm::ivec2 dimensions[MAX_LAYERS];
        int blendingModes[MAX_LAYERS];
        float alphas[MAX_LAYERS];
        float alpha = 1.0f;
        bool inverse = false;
        bool reducePalette = false;
        bool colorShift = false;
        bool colorShift2 = false;
    } mutable uniforms_;

    std::string name_;
    Layers layers_;
    Effects effects_;
    bool valid_;
    midiNote alphaControl_ = DEFAULT_MASTER_ALPHA_CONTROL;

};
