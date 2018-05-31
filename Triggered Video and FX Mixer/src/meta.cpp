#include "meta.h"

const int LayerDescription::invalid_id = -1;
const std::string LayerDescription::invalid_video = "";
const uint8_t LayerDescription::invalid_midi = 255;

bool LayerDescription::fromXml(ofxXmlSettings & config) {
    id = config.getValue("id", invalid_id);
    video = config.getValue("video", invalid_video);
    blendMode = static_cast<Layer::BlendMode>(config.getValue("blendMode", static_cast<int>(Layer::BlendMode::Invalid)));
    for (int i = 0; i < config.getNumTags("midi"); i++) {
        midiMap.insert(config.getValue("midi", invalid_midi, i));
    }

    if (id == invalid_id || video == invalid_video || blendMode == Layer::BlendMode::Invalid || midiMap.count(invalid_midi)) {
        ofLog(OF_LOG_ERROR, "Layer description contains invalid values and will be skipped.");
        return false;
    }

    return true;
}

void LayerDescription::toXml(ofxXmlSettings& config) const {
    config.setValue("id", id);
    config.setValue("video", video);
    config.setValue("blendMode", static_cast<int>(blendMode));
    for (auto midiNote : midiMap)
        config.addValue("midi", midiNote);
}

void SceneDescription::fromXml(ofxXmlSettings & config) {
    for (int i = 0; i < config.getNumTags("layer"); i++) {
        config.pushTag("layer", i);
        LayerDescription layer;
        if (layer.fromXml(config))
            layers.push_back(std::move(layer));
        config.popTag(); // scene
    }
}

void SceneDescription::toXml(ofxXmlSettings & config) const {
    for (int i = 0; i < layers.size(); i++) {
        config.addTag("layer");
        config.pushTag("layer", i);
        layers[i].toXml(config);
        config.popTag(); // layer
    }
}

void ShowDescription::fromXml(ofxXmlSettings & config) {
    for (int i = 0; i < config.getNumTags("scene"); i++) {
        config.pushTag("scene", i);
        SceneDescription scene;
        scene.fromXml(config);
        scenes.push_back(std::move(scene));
        config.popTag(); // scene
    }
}

void ShowDescription::toXml(ofxXmlSettings & config) const {
    for (int i = 0; i < scenes.size(); i++) {
        config.addTag("scene");
        config.pushTag("scene", i);
        scenes[i].toXml(config);
        config.popTag(); // scene
    }
}
