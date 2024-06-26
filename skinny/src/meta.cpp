#include "meta.h"
#include "Status.h"

namespace skinny {

static const std::filesystem::path invalid_path = {};
const uint8_t MappableDescription::invalid_midi = 255;

//--------------------------------------------------------------
MappableDescription::MappableDescription(midiNote note, midiNote cc) :
    note(note),
    cc(cc)
{}

//--------------------------------------------------------------
LayerDescription::LayerDescription(unsigned int id,
                                   const std::filesystem::path& path,
                                   midiNote note,
                                   midiNote cc,
                                   int alpha,
                                   const BlendMode& blendMode) :
    MappableDescription(
        note == invalid_midi ? id + LAYER_NOTE_OFFSET : note,
        cc == invalid_midi ? id + LAYER_CC_OFFSET : cc),
    id(id),
    path(path),
    alpha(alpha),
    blendMode(blendMode)
{
    valid = !(id >= MAX_LAYERS || path.empty() || blendMode == BlendMode::Invalid || this->note > MAX_7BIT);
    if (!valid)
        ofLog(OF_LOG_ERROR, "Layer description was initialized with invalid values.");
}

//--------------------------------------------------------------
bool LayerDescription::fromXml(ofxXmlSettings & config) {
    id = config.getValue("id", -1);
    path = config.getValue("path", invalid_path.string());
    blendMode = static_cast<BlendMode>(config.getValue("blendMode", static_cast<int>(BlendMode::Invalid)));
    cc = config.getValue("alphaControl", invalid_midi);
    alpha = config.getValue("alpha", MAX_7BIT);
    note = config.getValue("midi", invalid_midi);
    retrigger = config.getValue("retrigger", false);
    valid = !(id >= MAX_LAYERS || path.empty() || blendMode == BlendMode::Invalid || note == invalid_midi);

    if (!valid) {
        ofLog(OF_LOG_ERROR, "Layer description contains invalid values and will be skipped.");
    }

    return valid;
}

//--------------------------------------------------------------
void LayerDescription::toXml(ofxXmlSettings& config) const {
    config.setValue("id", static_cast<int>(id));
    config.setValue("path", path.string());
    config.setValue("blendMode", static_cast<int>(blendMode));
    config.setValue("alphaControl", cc);
    config.setValue("alpha", alpha);
    config.setValue("midi", note);
    config.setValue("retrigger", retrigger);
}

//--------------------------------------------------------------
EffectDescription::EffectDescription(int id, EffectType type, midiNote note, midiNote cc) :
    MappableDescription(
        note == invalid_midi ? static_cast<int>(type) + EFFECT_NOTE_OFFSET : note,
        cc == invalid_midi ? static_cast<int>(type) + EFFECT_CC_OFFSET : cc),
    id(id),
    type(type)
{
    valid = !(type == EffectType::Invalid || this->note > MAX_7BIT);
    if (!valid)
        ofLog(OF_LOG_ERROR, "Effect description was initialized with invalid values.");
}

//--------------------------------------------------------------
bool EffectDescription::fromXml(ofxXmlSettings & config) {
    id = config.getValue("id", -1);
    type = static_cast<EffectType>(config.getValue("type", static_cast<int>(EffectType::Invalid)));
    note = config.getValue("midi", invalid_midi);
    cc = config.getValue("cc", invalid_midi);
    param = config.getValue("param", MAX_7BIT);
    valid = !(type == EffectType::Invalid || note == invalid_midi);

    if (!valid) {
        ofLog(OF_LOG_ERROR, "Effect description contains invalid values and will be skipped.");
    }

    return valid;
}

//--------------------------------------------------------------
void EffectDescription::toXml(ofxXmlSettings & config) const {
    config.setValue("id", id);
    config.setValue("type", static_cast<int>(type));
    config.setValue("midi", note);
    config.setValue("cc", cc);
    config.setValue("param", param);
}

//--------------------------------------------------------------
SceneDescription::SceneDescription(const std::string& name) :
    name(name)
{}

bool SceneDescription::fromXml(ofxXmlSettings & config) {
    name = config.getValue("name", "");

    for (auto i = 0; i < config.getNumTags("layer"); i++) {
        config.pushTag("layer", i);
        LayerDescription layer;
        if (layer.fromXml(config))
            layers[layer.id] = std::move(layer);
        config.popTag(); // layer
    }

    return true; // #TODO check this somehow
}

//--------------------------------------------------------------
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

//--------------------------------------------------------------
ShowDescription::ShowDescription()
{
    // Create default scene -- show is valid / usable right from the start
    appendScene();
    for (auto i = 0; i < MAX_EFFECTS; i++) {
        // #TODO revise default effects
        effects_[i] = EffectDescription(i, static_cast<EffectType>(i));
    }
}

//--------------------------------------------------------------
bool ShowDescription::fromXml(ofxXmlSettings& config) {
    currentIdx_ = 0;
    scenes_.clear();
    // #TODO check for pushTag() etc return values

    config.pushTag("head");
    switchNoteForward_ = config.getValue("switchNoteForward", MappableDescription::invalid_midi);
    switchNoteBackward_ = config.getValue("switchNoteBackward", MappableDescription::invalid_midi);
    midiChannel_ = config.getValue("channel", default_channel);
    alphaControl_ = config.getValue("masterAlphaControl", DEFAULT_MASTER_ALPHA_CONTROL);
    spoutOut_ = config.getValue("spoutOut", false);

    config.pushTag("loadingScreens");
    loadingScreensPath_ = config.getValue("path", invalid_path.string());
    loadingScreensNote_ = config.getValue("midi", MappableDescription::invalid_midi);
    loadingScreensCc_ = config.getValue("cc", MappableDescription::invalid_midi);
    config.popTag(); // loadingScreens
    config.popTag(); // head

    config.pushTag("show");

    for (auto i = 0; i < config.getNumTags("scene"); i++) {
        config.pushTag("scene", i);
        SceneDescription scene;
        scene.fromXml(config);
        scenes_.emplace_back(scene);
        config.popTag(); // scene
    }

    config.pushTag("effects");
    if (config.getNumTags("effect") == 0) {
        // create default effects
        // #TODO revise default effects
        for (auto i = 0; i < MAX_EFFECTS; i++) {
            effects_[i] = EffectDescription(i, static_cast<EffectType>(i));
        }
    }
    else {
        for (auto i = 0; i < config.getNumTags("effect"); i++) {
            config.pushTag("effect", i);
            EffectDescription effect;
            if (effect.fromXml(config))
                effects_[i] = effect;
            config.popTag(); // effect
        }
    }
    config.popTag(); // effects

    config.popTag(); // show

    return true;
}

//--------------------------------------------------------------
void ShowDescription::toXml(ofxXmlSettings& config) const {
    config.addTag("head");
    config.pushTag("head");
    config.setValue("version", VERSION);
    config.setValue("switchNoteForward", switchNoteForward_);
    config.setValue("switchNoteBackward", switchNoteBackward_);
    config.setValue("channel", midiChannel_);
    config.addValue("masterAlphaControl", alphaControl_);
    config.addValue("spoutOut", spoutOut_);

    config.addTag("loadingScreens");
    config.pushTag("loadingScreens");
    config.setValue("path", loadingScreensPath_.string());
    config.setValue("midi", loadingScreensNote_);
    config.setValue("cc", loadingScreensCc_);
    config.popTag(); // loadingScreens
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
}

//--------------------------------------------------------------
void ShowDescription::setup()
{
	ofAddListener(getStatus().midi->controlChangeEvent, this, &ShowDescription::onControlChange);
}

//--------------------------------------------------------------
void ShowDescription::exit()
{
  ofRemoveListener(getStatus().midi->controlChangeEvent, this, &ShowDescription::onControlChange);
}

//--------------------------------------------------------------
void ShowDescription::onControlChange(ControlChangeMessage& msg)
{
  if (msg.channel_ != midiChannel_)
    return;

  for (auto& layer : scenes_[currentIdx_].layers)
  {
    if (layer.cc == msg.control_)
    {
      layer.alpha = msg.value_;
    }
	}

	for (auto& effect : effects_)
	{
		if (effect.cc == msg.control_)
		{
			effect.param = msg.value_;
		}
	}
}

//--------------------------------------------------------------
bool ShowDescription::shift(LoadDir dir, int idx)
{
    const auto prevIdx = currentIdx_;
    switch (dir)
    {
    case LoadDir::CurrentLayers:
    case LoadDir::CurrentEffects:
    case LoadDir::CurrentAll:
        return true;
    case LoadDir::Forward:
        currentIdx_ = ++currentIdx_ % scenes_.size();
        break;
    case LoadDir::Backward:
        --currentIdx_;
        if (currentIdx_ < 0)
            currentIdx_ = currentIdx_ + static_cast<int>(scenes_.size());
        else
            currentIdx_ = currentIdx_ % static_cast<int>(scenes_.size());
        break;
    case LoadDir::Jump:
        if (idx >= 0 && idx < scenes_.size()) {
					currentIdx_ = idx;
        }
        break;
    default:
        ofLog(ofLogLevel::OF_LOG_ERROR, "Tried to shift description with invalid enum.");
        return false;
    }

    return prevIdx != currentIdx_;
}

//--------------------------------------------------------------
void ShowDescription::appendScene(const std::string& name)
{
    scenes_.push_back(SceneDescription(name));
}

} // namespace skinny
