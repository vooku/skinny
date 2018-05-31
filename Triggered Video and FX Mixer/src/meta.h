#pragma once

#include <set>
#include <vector>
#include "ofxXmlSettings.h"
#include "Layer.h"

struct LayerDescription {
    //void fromXml();
    void toXml(ofxXmlSettings& config) const;

    int id;
    std::string video;
    std::set<int> midiMap;
    Layer::BlendMode blendMode;
};

struct SceneDescription {
    //void fromXml();
    void toXml(ofxXmlSettings& config) const;

    std::vector<LayerDescription> layers;
};

struct ShowDescription {
    void fromXml(const ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    std::vector<SceneDescription> scenes;
};