#pragma once

#include <set>
#include <vector>
#include <array>
#include <filesystem>
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
    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    Effect::Type type;
    Mappable::MidiMap midiMap;
};

struct SceneDescription {
    void fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    std::string name;
    std::array<LayerDescription, MAX_LAYERS> layers;
    std::vector<EffectDescription> effects;
};

class ShowDescription {
public:
    friend class Gui;

    ShowDescription() : currentIdx_(0) { }

    void fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;
    ShowDescription& operator++();
    ShowDescription& operator--();

    const SceneDescription& currentScene() const { return scenes_[currentIdx_]; }
    auto getSize() { return scenes_.size(); }

private:
    std::vector<SceneDescription> scenes_;
    int currentIdx_;
};