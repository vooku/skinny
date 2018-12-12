#pragma once

#include <set>
#include <vector>
#include <array>
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
                     const Mappable::MidiMap& midiMap = {},
                     const Layer::BlendMode& blendMode = Layer::BlendMode::Normal);

    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    unsigned int id;
    std::filesystem::path path;
    Mappable::MidiMap midiMap;
    Layer::BlendMode blendMode;
    bool valid = false;
};

struct EffectDescription : public MappableDescription {
    EffectDescription() = default;
    explicit EffectDescription(Effect::Type type) : type(type) { }

    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    Effect::Type type;
    Mappable::MidiMap midiMap;
};

struct SceneDescription {
    SceneDescription() = default;
    explicit SceneDescription(const std::string& name);

    void fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    std::string name;
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