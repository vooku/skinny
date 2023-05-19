#pragma once

// required for TARGET_WIN32
#include "ofConstants.h"

#ifdef TARGET_WIN32
#include "ofxSpout.h"
#endif

#include "Scene.h"
#include "Mappable.h"

namespace skinny {

class Gui;

typedef std::shared_ptr<Scene> ScenePtr;

class Show : public Mappable
{
public:
    typedef std::array<std::unique_ptr<Effect>, MAX_EFFECTS> Effects;

    friend class Gui;

    Show(int width, int height);

    virtual void setup() override;
    virtual void exit() override;
    void draw();
    bool reload(const ShowDescription& description);
		bool reloadLayers(const ShowDescription& description);
		bool reloadEffects(const ShowDescription& description);
    void playPauseEffect(int i);
    void update();

    virtual void onNoteOn(NoteMessage& msg) override;
    virtual void onControlChange(ControlChangeMessage& msg) override;

    ScenePtr getCurrentScene() const;
    const Effects& getEffects() const;
    midiNote getAlphaControl() const;
    float getAlpha() const;

    void setAlphaControl(const midiNote& control);

    const ofPixels& getSubsampledTexture() const;

private:
    struct Uniforms {
        int fxTypes[MAX_EFFECTS];
        int fxPlaying[MAX_EFFECTS];
        float fxParam[MAX_EFFECTS];
        float masterAlpha_ = 1.0f;
    } mutable uniforms_;

    bool loadShaders();

    void setupFirstPassUniforms() const;
    void setupPingPongPassUniforms(bool horizontal, const ofTexture& img) const;
    void setupSubsamplePassUniforms(const ofTexture& img) const;
    bool hasActiveFX() const;

		const ofTexture& getFinalTexture() const;

    mutable ofShader firstPassShader_;
    mutable ofShader pingPongPassShader_;
    mutable ofShader subsamplePassShader_;
    std::array<ofFbo, 4> fbos_;

    ofPixels subsampledPixels_;

    const int width_, height_;
    ScenePtr currentScene_;

    midiNote masterAlphaControl_ = DEFAULT_MASTER_ALPHA_CONTROL;

    Effects effects_;

#ifdef TARGET_WIN32
    ofxSpout::Sender spoutSender_;
#endif
};

} // namespace skinny
