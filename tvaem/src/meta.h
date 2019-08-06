#pragma once

#include <vector>
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
                     midiNote note = invalid_midi,
                     midiNote alphaControl = invalid_midi,
                     //float alpha = 1.0f,
                     const Layer::BlendMode& blendMode = Layer::BlendMode::Normal);

    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    unsigned int id;
    std::filesystem::path path;
    midiNote note;
    midiNote alphaControl;
    //float alpha;
    bool retrigger = false;
    Layer::BlendMode blendMode;
    bool valid = false;
};

struct EffectDescription : public MappableDescription {
    EffectDescription() = default;
    explicit EffectDescription(Effect::Type type, midiNote note = invalid_midi);

    bool fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    Effect::Type type;
    midiNote note;
    bool valid = false;
};

struct SceneDescription {
    SceneDescription() = default;
    SceneDescription(const std::string& name);

    void fromXml(ofxXmlSettings& config);
    void toXml(ofxXmlSettings& config) const;

    std::string name;
    std::array<LayerDescription, MAX_LAYERS> layers;
};

class ShowDescription {
public:
    friend class Gui;

    ShowDescription();

    bool fromXml(const std::string& filename);
    bool toXml(const std::string& filename) const;
    ShowDescription& operator++();
    ShowDescription& operator--();
    void appendScene(const std::string& name = "New scene");

    const SceneDescription& currentScene() const { return scenes_[currentIdx_]; }
    auto getSceneIndex() const { return currentIdx_; }
    auto getSize() const { return scenes_.size(); }
    auto getSwitchNote() const { return switchNote_; }
    auto getMidiChannel() const { return midiChannel_; }
    const auto& getEffects() const { return effects_; }
    auto getAlphaControl() const { return alphaControl_; }

    void setMidiChannel(int channel) { midiChannel_ = channel; }

private:
    static const int default_channel = 2;

    std::vector<SceneDescription> scenes_;
    std::array<EffectDescription, MAX_EFFECTS> effects_;

    int currentIdx_ = 0;
    midiNote switchNote_ = MappableDescription::invalid_midi;
    int midiChannel_ = default_channel;
    midiNote alphaControl_ = DEFAULT_MASTER_ALPHA_CONTROL;

};