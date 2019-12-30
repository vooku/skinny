#pragma once

#include <vector>
#include "ofxXmlSettings.h"
#include "Layer.h"
#include "Effect.h"
#include "base.h"

namespace skinny {

struct Serialiazable {
    virtual bool fromXml(ofxXmlSettings& config) = 0;
    virtual void toXml(ofxXmlSettings& config) const = 0;
};

struct MappableDescription : public Serialiazable {
    MappableDescription() = default;
    MappableDescription(midiNote note, midiNote cc);

    bool fromXml(ofxXmlSettings& config) override = 0;
    void toXml(ofxXmlSettings& config) const override = 0;

    static const uint8_t invalid_midi;
    midiNote note, cc;
    bool valid = false;
};

struct LayerDescription : public MappableDescription {
    static const std::filesystem::path invalid_path;

    LayerDescription() = default;
    LayerDescription(unsigned int id,
                     const std::filesystem::path& path,
                     midiNote note = invalid_midi,
                     midiNote cc = invalid_midi,
                     //float alpha = 1.0f,
                     const BlendMode& blendMode = BlendMode::Normal);

    bool fromXml(ofxXmlSettings& config) override;
    void toXml(ofxXmlSettings& config) const override;

    unsigned int id;
    std::filesystem::path path;
    //float alpha;
    bool retrigger = false;
    BlendMode blendMode;
};

struct EffectDescription : public MappableDescription {
    EffectDescription() = default;
    EffectDescription(int id, EffectType type, midiNote note = invalid_midi, midiNote cc = invalid_midi);

    bool fromXml(ofxXmlSettings& config) override;
    void toXml(ofxXmlSettings& config) const override;

    int id;
    EffectType type;
    int param = 127;
};

struct SceneDescription : public Serialiazable {
    SceneDescription() = default;
    SceneDescription(const std::string& name);

    bool fromXml(ofxXmlSettings& config) override;
    void toXml(ofxXmlSettings& config) const override;

    std::string name;
    std::array<LayerDescription, MAX_LAYERS> layers;
};

class ShowDescription : public Serialiazable {
public:
    friend class Gui;

    ShowDescription();

    bool fromXml(ofxXmlSettings& config) override;
    void toXml(ofxXmlSettings& config) const override;
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

} // namespace skinny
