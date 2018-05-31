#include "meta.h"

void LayerDescription::toXml(ofxXmlSettings& config) const {
    config.setValue("id", id);
    config.setValue("video", video);
    config.setValue("blendingMode", static_cast<int>(blendMode));
    for (auto midiNote : midiMap)
        config.addValue("midi", midiNote);
}

void SceneDescription::toXml(ofxXmlSettings & config) const {
    for (int i = 0; i < layers.size(); i++) {
        config.addTag("layer");
        config.pushTag("layer", i);
        layers[i].toXml(config);
        config.popTag(); // layer
    }
}

void ShowDescription::fromXml(const ofxXmlSettings & config) {
}

void ShowDescription::toXml(ofxXmlSettings & config) const {
    for (int i = 0; i < scenes.size(); i++) {
        config.addTag("scene");
        config.pushTag("scene", i);
        scenes[i].toXml(config);
        config.popTag(); // scene
    }
}
