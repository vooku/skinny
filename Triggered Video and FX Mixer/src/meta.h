#pragma once

#include <set>
#include <vector>
#include "ofxXmlSettings.h"
#include "Layer.h"

struct LayerDescription {
    static const int invalid_id;
    static const std::string invalid_video;
    static const uint8_t invalid_midi;
    
    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    int id;
    std::string video;
    Layer::MidiMap midiMap;
    Layer::BlendMode blendMode;
};

struct SceneDescription {
    void fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    std::vector<LayerDescription> layers;
};

struct ShowDescription {
    void fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    std::vector<SceneDescription> scenes;
};