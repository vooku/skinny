#include "meta.h"

const uint8_t MappableDescription::invalid_midi = 255;
const std::filesystem::path LayerDescription::invalid_path = {};

LayerDescription::LayerDescription(unsigned int id,
                                   const std::filesystem::path& path,
                                   const Mappable::MidiMap& midiMap,
                                   const Layer::BlendMode& blendMode) :
    id(id),
    path(path),
    midiMap(midiMap),
    blendMode(blendMode)
{
    valid = !(id >= MAX_LAYERS || path.empty() || blendMode == Layer::BlendMode::Invalid || midiMap.count(invalid_midi) > 0);
    if (!valid)
        ofLog(OF_LOG_ERROR, "Layer description was initialized with invalid values.");
}

bool LayerDescription::fromXml(ofxXmlSettings & config) {
    valid = false;

    id = config.getValue("id", -1);
    path = config.getValue("path", invalid_path.string());
    blendMode = static_cast<Layer::BlendMode>(config.getValue("blendMode", static_cast<int>(Layer::BlendMode::Invalid)));

    for (int i = 0; i < config.getNumTags("midi"); i++) {
        midiMap.insert(config.getValue("midi", invalid_midi, i));
    }

    if (id >= MAX_LAYERS || path.empty() || blendMode == Layer::BlendMode::Invalid || midiMap.count(invalid_midi)) {
        ofLog(OF_LOG_ERROR, "Layer description contains invalid values and will be skipped.");
        return false;
    }

    valid = true;
    return true;
}

void LayerDescription::toXml(ofxXmlSettings& config) const {
    config.setValue("id", static_cast<int>(id));
    config.setValue("path", path.string());
    config.setValue("blendMode", static_cast<int>(blendMode));
    for (auto midiNote : midiMap)
        config.addValue("midi", midiNote);
}

bool EffectDescription::fromXml(ofxXmlSettings & config) {
    type = static_cast<Effect::Type>(config.getValue("type", static_cast<int>(Effect::Type::Invalid)));
    for (int i = 0; i < config.getNumTags("midi"); i++) {
        midiMap.insert(config.getValue("midi", invalid_midi, i));
    }

    if (type == Effect::Type::Invalid || midiMap.count(invalid_midi)) {
        ofLog(OF_LOG_ERROR, "Effect description contains invalid values and will be skipped.");
        return false;
    }

    return true;
}

void EffectDescription::toXml(ofxXmlSettings & config) const {
    config.setValue("type", static_cast<int>(type));
    for (auto midiNote : midiMap)
        config.addValue("midi", midiNote);
}


SceneDescription::SceneDescription(const std::string & name) :
    name(name)
{
    for (int i = 0; i < static_cast<int>(Effect::Type::Count); i++) {
        effects.push_back({ static_cast<Effect::Type>(i) });
    }
}

void SceneDescription::fromXml(ofxXmlSettings & config) {
    name = config.getValue("name", "");

    for (int i = 0; i < config.getNumTags("layer"); i++) {
        config.pushTag("layer", i);
        LayerDescription layer;
        if (layer.fromXml(config))
            layers[layer.id] = std::move(layer);
        config.popTag(); // layer
    }

    for (int i = 0; i < config.getNumTags("effect"); i++) {
        config.pushTag("effect", i);
        EffectDescription effect;
        if (effect.fromXml(config))
            effects.push_back(std::move(effect));
        config.popTag(); // effect
    }
}

void SceneDescription::toXml(ofxXmlSettings & config) const {
    config.addValue("name", name);

    int validLayers = 0;
    for (const auto& layer : layers) {
        if (layer.valid) {
            config.addTag("layer");
            config.pushTag("layer", validLayers++);
            layer.toXml(config);
            config.popTag(); // layer
        }
    }

    for (int i = 0; i < effects.size(); i++) {
        config.addTag("effect");
        config.pushTag("effect", i);
        effects[i].toXml(config);
        config.popTag(); // effect
    }
}

bool ShowDescription::fromXml(const std::string& filename) {
    ofxXmlSettings config;
    if (!config.loadFile(filename)) {
        return false;
    }

    currentIdx_ = 0;
    scenes_.clear();
    // TODO check for pushTag() etc return values

    config.pushTag("head");
    switchNote_ = config.getValue("switchNote", MappableDescription::invalid_midi);
    config.popTag(); // head

    config.pushTag("show");
    for (int i = 0; i < config.getNumTags("scene"); i++) {
        config.pushTag("scene", i);
        SceneDescription scene;
        scene.fromXml(config);
        scenes_.push_back(std::move(scene));
        config.popTag(); // scene
    }
    config.popTag(); // show

    return true;
}

bool ShowDescription::toXml(const std::string& filename) const {
    ofxXmlSettings config;
    config.addTag("head");
    config.pushTag("head");
    config.setValue("version", version);
    config.setValue("switchNote", switchNote_);
    config.popTag(); // head

    config.addTag("show");
    config.pushTag("show");
    for (int i = 0; i < scenes_.size(); i++) {
        config.addTag("scene");
        config.pushTag("scene", i);
        scenes_[i].toXml(config);
        config.popTag(); // scene
    }
    config.popTag(); // show

    return config.saveFile(filename);
}

ShowDescription & ShowDescription::operator++() {
    currentIdx_ = ++currentIdx_ % scenes_.size();
    return *this;
}

ShowDescription & ShowDescription::operator--()
{
    --currentIdx_;
    if (currentIdx_ < 0)
        currentIdx_ = currentIdx_ + scenes_.size();
    else
        currentIdx_ = currentIdx_ % scenes_.size();
    return *this;
}

void ShowDescription::appendScene(const std::string& name)
{
    scenes_.push_back({ name });
}
