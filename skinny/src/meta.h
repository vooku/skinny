#pragma once

#include <vector>
#include "ofxXmlSettings.h"
#include "VideoLayer.h"
#include "Effect.h"
#include "base.h"

namespace skinny {

struct Serializable {
    virtual bool fromXml(ofxXmlSettings& config) = 0;
    virtual void toXml(ofxXmlSettings& config) const = 0;
};

struct MappableDescription : public Serializable {
    MappableDescription() = default;
    MappableDescription(midiNote note, midiNote cc);

    bool fromXml(ofxXmlSettings& config) override = 0;
    void toXml(ofxXmlSettings& config) const override = 0;

    static const uint8_t invalid_midi;
    midiNote note = invalid_midi;
    midiNote cc = invalid_midi;
    bool valid = false;
};

struct LayerDescription : public MappableDescription {
    LayerDescription() = default;
    LayerDescription(unsigned int id,
                     const std::filesystem::path& path,
                     midiNote note = invalid_midi,
                     midiNote cc = invalid_midi,
                     int alpha = MAX_7BIT,
                     const BlendMode& blendMode = DEFAULT_BLEND_MODE);

    bool fromXml(ofxXmlSettings& config) override;
    void toXml(ofxXmlSettings& config) const override;

    unsigned int id = 0;
    std::filesystem::path path;
    int alpha = MAX_7BIT;
    bool retrigger = false;
    BlendMode blendMode = BlendMode::Invalid;
};

struct EffectDescription : public MappableDescription {
    EffectDescription() = default;
    EffectDescription(int id, EffectType type, midiNote note = invalid_midi, midiNote cc = invalid_midi);

    bool fromXml(ofxXmlSettings& config) override;
    void toXml(ofxXmlSettings& config) const override;

    int id = 0;
    EffectType type = EffectType::Invalid;
    int param = MAX_7BIT;
};

struct SceneDescription : public Serializable {
    SceneDescription() = default;
    SceneDescription(const std::string& name);

    bool fromXml(ofxXmlSettings& config) override;
    void toXml(ofxXmlSettings& config) const override;

    std::string name;
    std::array<LayerDescription, MAX_LAYERS> layers;
};

class ShowDescription : public Serializable {
public:
    friend class Gui;

    ShowDescription();

    bool fromXml(ofxXmlSettings& config) override;
    void toXml(ofxXmlSettings& config) const override;

    // assign CC to layers / effects on the fly
		void setup();
		void exit();
		void onControlChange(ControlChangeMessage& msg);

    /**
     * @return True if the current scene index was changed.
     */
    bool shift(LoadDir dir, int idx = -1);
    void appendScene(const std::string& name = "Enter name");

    const SceneDescription& currentScene() const { return scenes_[currentIdx_]; }
    auto getSceneIndex() const { return currentIdx_; }
    auto getSize() const { return scenes_.size(); }
    auto getSwitchNoteForward() const { return switchNoteForward_; }
    auto getSwitchNoteBackward() const { return switchNoteBackward_; }
    auto getMidiChannel() const { return midiChannel_; }
    const auto& getEffects() const { return effects_; }
    auto getAlphaControl() const { return alphaControl_; }
    auto getSpoutOut() const { return spoutOut_; }

    const auto& getLoadingScreensPath() const { return loadingScreensPath_; }
    auto getLoadingScreensNote() const { return loadingScreensNote_; }
    auto getLoadingScreensCc() const { return loadingScreensCc_; }

    void setMidiChannel(int channel) { midiChannel_ = channel; }

private:
    static const int default_channel = 2;

    std::vector<SceneDescription> scenes_;
    std::array<EffectDescription, MAX_EFFECTS> effects_;

    int currentIdx_ = 0;

    midiNote  switchNoteForward_ = MappableDescription::invalid_midi;
    midiNote  switchNoteBackward_ = MappableDescription::invalid_midi;
    int       midiChannel_ = default_channel;
    midiNote  alphaControl_ = DEFAULT_MASTER_ALPHA_CONTROL;
    bool      spoutOut_ = false;

    std::filesystem::path loadingScreensPath_;
    midiNote loadingScreensNote_ = MappableDescription::invalid_midi;
    midiNote loadingScreensCc_ = MappableDescription::invalid_midi;

};

} // namespace skinny
