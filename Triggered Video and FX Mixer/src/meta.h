#pragma once

#include <set>
#include <vector>
#include "ofxXmlSettings.h"
#include "Layer.h"
#include "Effect.h"

struct MappableDescription {
    static const uint8_t invalid_midi;
};

struct LayerDescription : public MappableDescription {
    static const int invalid_id;
    static const std::string invalid_video;
    
    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    int id;
    std::string video;
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
    ShowDescription() : currentIdx_(0), nextIdx_(0) { }

    void fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;
    ShowDescription& operator++();

    const SceneDescription& currentScene() const { return scenes_[currentIdx_]; }
    const SceneDescription& nextScene() const { return scenes_[nextIdx_]; }
    size_t getSize() { return scenes_.size(); }

private:
    std::vector<SceneDescription> scenes_;
    int currentIdx_, nextIdx_;
};