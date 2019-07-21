#pragma once

#include "OfxMidi.h"
#include "meta.h"

class Gui;

class Scene {
public:
    typedef std::array<std::unique_ptr<Layer>, MAX_LAYERS> Layers;

    friend class Gui;

    void reload(const SceneDescription& description);
    void bind();
    void unbind();
    bool isFrameNew();

    /**
     * Return which layers / effects were (de)activated.
     */
    struct FoundMappables {
        std::vector<std::pair<int, bool>> layers;
        std::vector<std::pair<int, bool>> effects;
    };
    FoundMappables newMidiMessage(const ofxMidiMessage & msg);

    void playPauseLayer(int idx);

    void setupUniforms(ofShader& shader) const;

    void setAlpha(int alpha) const { uniforms_.alpha = (alpha < 0 ? 0 : alpha > 127 ? 127 : alpha) / 127.0f; }
    void setAlphaControl(midiNote control) { alphaControl_ = control; }

    const auto& getName() const { return name_; }
    const auto& getLayers() const { return layers_; }
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
    } mutable uniforms_;

    std::string name_;
    Layers layers_;
    bool valid_;
    midiNote alphaControl_ = DEFAULT_MASTER_ALPHA_CONTROL;

};
