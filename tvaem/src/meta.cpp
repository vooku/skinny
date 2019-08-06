#include "meta.h"

const uint8_t MappableDescription::invalid_midi = 255;
const std::filesystem::path LayerDescription::invalid_path = {};

LayerDescription::LayerDescription(unsigned int id,
                                   const std::filesystem::path& path,
                                   midiNote note,
                                   midiNote cc,
                                   //float alpha,
                                   const Layer::BlendMode& blendMode) :
    id(id),
    path(path),
    note(note == invalid_midi ? id + Layer::MIDI_OFFSET : note),
    cc(cc == invalid_midi ? id + Layer::ALPHA_MIDI_OFFSET : cc),
    //alpha(alpha),
    blendMode(blendMode)
{
    valid = !(id >= MAX_LAYERS || path.empty() || blendMode == Layer::BlendMode::Invalid || this->note > 127);
    if (!valid)
        ofLog(OF_LOG_ERROR, "Layer description was initialized with invalid values.");
}

bool LayerDescription::fromXml(ofxXmlSettings & config) {
    id = config.getValue("id", -1);
    path = config.getValue("path", invalid_path.string());
    blendMode = static_cast<Layer::BlendMode>(config.getValue("blendMode", static_cast<int>(Layer::BlendMode::Invalid)));
    cc = config.getValue("alphaControl", static_cast<int>(id + Layer::ALPHA_MIDI_OFFSET));
    note = config.getValue("midi", invalid_midi);
    retrigger = config.getValue("retrigger", false);
    valid = !(id >= MAX_LAYERS || path.empty() || blendMode == Layer::BlendMode::Invalid || note == invalid_midi);

    if (!valid) {
        ofLog(OF_LOG_ERROR, "Layer description contains invalid values and will be skipped.");
    }

    return valid;
}

void LayerDescription::toXml(ofxXmlSettings& config) const {
    config.setValue("id", static_cast<int>(id));
    config.setValue("path", path.string());
    config.setValue("blendMode", static_cast<int>(blendMode));
    config.setValue("alphaControl", cc);
    config.setValue("midi", note);
    config.setValue("retrigger", retrigger);
}

EffectDescription::EffectDescription(Effect::Type type, midiNote note, midiNote cc) :
    type(type),
    note(note == invalid_midi ? static_cast<int>(type) + Effect::MIDI_OFFSET : note),
    cc(cc == invalid_midi ? static_cast<int>(type) + Effect::MIDI_OFFSET : cc)
{
    valid = !(type == Effect::Type::Invalid || this->note > 127);
    if (!valid)
        ofLog(OF_LOG_ERROR, "Effect description was initialized with invalid values.");
}

bool EffectDescription::fromXml(ofxXmlSettings & config) {
    type = static_cast<Effect::Type>(config.getValue("type", static_cast<int>(Effect::Type::Invalid)));
    note = config.getValue("midi", invalid_midi);
    valid = !(type == Effect::Type::Invalid || note == invalid_midi);

    if (!valid) {
        ofLog(OF_LOG_ERROR, "Effect description contains invalid values and will be skipped.");
    }

    return valid;
}

void EffectDescription::toXml(ofxXmlSettings & config) const {
    config.setValue("type", static_cast<int>(type));
    config.setValue("midi", note);
}


SceneDescription::SceneDescription(const std::string& name) :
    name(name)
{}

void SceneDescription::fromXml(ofxXmlSettings & config) {
    name = config.getValue("name", "");

    for (auto i = 0; i < config.getNumTags("layer"); i++) {
        config.pushTag("layer", i);
        LayerDescription layer;
        if (layer.fromXml(config))
            layers[layer.id] = std::move(layer);
        config.popTag(); // layer
    }
}

void SceneDescription::toXml(ofxXmlSettings & config) const {
    config.addValue("name", name);

    auto validLayers = 0;
    for (const auto& layer : layers) {
        if (layer.valid) {
            config.addTag("layer");
            config.pushTag("layer", validLayers++);
            layer.toXml(config);
            config.popTag(); // layer
        }
    }
}

ShowDescription::ShowDescription()
{
    // Create default scene -- show is valid / usable right from the start
    appendScene();
    for (auto i = 0; i < MAX_EFFECTS; i++) {
        effects_[i] = EffectDescription(static_cast<Effect::Type>(i));
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
    midiChannel_ = config.getValue("channel", default_channel);
    alphaControl_ = config.getValue("masterAlphaControl", DEFAULT_MASTER_ALPHA_CONTROL);
    config.popTag(); // head

    config.pushTag("show");

    for (auto i = 0; i < config.getNumTags("scene"); i++) {
        config.pushTag("scene", i);
        SceneDescription scene;
        scene.fromXml(config);
        scenes_.push_back(std::move(scene));
        config.popTag(); // scene
    }

    config.pushTag("effects");
    if (config.getNumTags("effect") == 0) {
        // create default effects
        // #TODO revise default effects
        for (auto i = 0; i < MAX_EFFECTS; i++) {
            effects_[i] = EffectDescription(static_cast<Effect::Type>(i));
        }
    }
    else {
        for (auto i = 0; i < config.getNumTags("effect"); i++) {
            config.pushTag("effect", i);
            EffectDescription effect;
            if (effect.fromXml(config))
                effects_[i] = std::move(effect);
            config.popTag(); // effect
        }
    }
    config.popTag(); // effects

    config.popTag(); // show

    return true;
}

bool ShowDescription::toXml(const std::string& filename) const {
    ofxXmlSettings config;
    config.addTag("head");
    config.pushTag("head");
    config.setValue("version", VERSION);
    config.setValue("switchNote", switchNote_);
    config.setValue("channel", midiChannel_);
    config.addValue("masterAlphaControl", alphaControl_);
    config.popTag(); // head

    config.addTag("show");
    config.pushTag("show");

    config.addTag("effects");
    config.pushTag("effects");
    auto validEffects = 0;
    for (const auto& effect : effects_) {
        if (effect.valid) {
            config.addTag("effect");
            config.pushTag("effect", validEffects++);
            effect.toXml(config);
            config.popTag(); // effect
        }
    }
    config.popTag(); // effects

    for (auto i = 0; i < scenes_.size(); i++) {
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
        currentIdx_ = currentIdx_ + static_cast<int>(scenes_.size());
    else
        currentIdx_ = currentIdx_ % static_cast<int>(scenes_.size());
    return *this;
}

void ShowDescription::appendScene(const std::string& name)
{
    scenes_.push_back(SceneDescription(name));
}
