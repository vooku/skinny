#pragma once

#include <set>
#include <vector>
#include "ofxXmlSettings.h"
#include "Layer.h"
#include "Effect.h"
#include "base.h"

struct MappableDescription {
    static const uint8_t invalid_midi;
};

struct LayerDescription : public MappableDescription {
    static const std::filesystem::path invalid_path;

    LayerDescription() = default;
    LayerDescription(unsigned int id,
                     const std::filesystem::path& path,
                     midiNote note = invalid_midi,
                     midiNote alphaControl = -1,
                     //float alpha = 1.0f,
                     const Layer::BlendMode& blendMode = Layer::BlendMode::Normal);

    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    unsigned int id;
    std::filesystem::path path;
    midiNote note;
    midiNote alphaControl;
    //float alpha;
    Layer::BlendMode blendMode;
    bool valid = false;
};

struct EffectDescription : public MappableDescription {
    EffectDescription() = default;
    explicit EffectDescription(Effect::Type type, midiNote note = invalid_midi) : type(type), note(note) { }

    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    Effect::Type type;
    midiNote note;
};

struct SceneDescription {
    SceneDescription() = default;
    SceneDescription(const std::string& name, midiNote alphaControl = DEFAULT_MASTER_ALPHA_CONTROL);

    void fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    std::string name;
    midiNote alphaControl;
    std::array<LayerDescription, MAX_LAYERS> layers;
    std::vector<EffectDescription> effects;
};

class ShowDescription {
public:
    friend class Gui;

    bool fromXml(const std::string& filename);
    bool toXml(const std::string& filename) const;
    ShowDescription& operator++();
    ShowDescription& operator--();
    void appendScene(const std::string& name = "New scene");

    const SceneDescription& currentScene() const { return scenes_[currentIdx_]; }
    auto getSize() const { return scenes_.size(); }
    auto getSwitchNote() const { return switchNote_; }

private:
    std::vector<SceneDescription> scenes_;
    int currentIdx_ = 0;
    midiNote switchNote_ = MappableDescription::invalid_midi;
};