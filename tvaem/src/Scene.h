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
    void update();

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

    const auto& getName() const { return name_; }
    const auto& getLayers() const { return layers_; }
    bool isValid() const { return valid_; }

private:
    struct Uniforms {
        int playing[MAX_LAYERS];
        glm::vec2 dimensions[MAX_LAYERS];
        int blendingModes[MAX_LAYERS];
        float alphas[MAX_LAYERS];
    } mutable uniforms_;

    std::string name_;
    Layers layers_;
    bool valid_;

};
