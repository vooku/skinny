#pragma once

#include <set>
#include <vector>
#include <filesystem>
#include "ofxXmlSettings.h"
#include "Layer.h"
#include "Effect.h"

struct MappableDescription {
    static const uint8_t invalid_midi;
};

struct LayerDescription : public MappableDescription {
    static const std::filesystem::path invalid_path;

    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    std::filesystem::path path;
    Mappable::MidiMap midiMap;
    Layer::BlendMode blendMode;
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
    std::vector<LayerDescription> layers;
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