#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Scene.h"
#include <array>
#include <experimental/filesystem>

class Gui {
public:
    static const ofColor BACKGROUND_COLOR;

    Gui() = delete;
    explicit Gui(ShowDescription* show);

    void setup();
    void draw() const;
    void reload(Scene* newScene);
    void update();
    void setActive(int layerId, bool active);
    void setActive(Effect::Type type, bool active);

    void onLayerButton(ofxDatGuiButtonEvent e);
    void onEffectButton(ofxDatGuiButtonEvent e);
    void onControlButton(ofxDatGuiButtonEvent e);

    void onLayerMidiInput(ofxDatGuiTextInputEvent e);
    void onLayerCcInput(ofxDatGuiTextInputEvent e);
    void onMasterAlphaCcInput(ofxDatGuiTextInputEvent e);
    void onEffectMidiInput(ofxDatGuiTextInputEvent e);
    void onSceneNameInput(ofxDatGuiTextInputEvent e);
    void onMidiChannelInput(ofxDatGuiTextInputEvent e);

    void onBlendModeDropdown(ofxDatGuiDropdownEvent e);

    void onLayerPlayToggle(ofxDatGuiToggleEvent e);
    void onEffectPlayToggle(ofxDatGuiToggleEvent e);
    void onLayerMuteToggle(ofxDatGuiToggleEvent e);
    void onEffectMuteToggle(ofxDatGuiToggleEvent e);
    void onLayerRetriggerToggle(ofxDatGuiToggleEvent e);

private:
    static const int MAX_CHARS = 20;
    static const int DELTA = 25;

    // control button names
    struct Btn {
        static const std::string NEXT;
        static const std::string PREV;
        static const std::string APPEND;
        static const std::string SAVE;
        static const std::string SAVE_AS;
        static const std::string LOAD;
    };

    class CommonTheme : public ofxDatGuiTheme {
    public:
        CommonTheme();
    } commonTheme_;

    class HeaderTheme : public CommonTheme {
    public:
        HeaderTheme();
    } headerTheme_;

    struct Fonts {
        static const int sizeRegular = 12;
        static const int sizeItalic = 14;
        ofTrueTypeFont regular, italic;
    } fonts_;

    void setupControlPanel(glm::ivec2& pos);
    void setupPlayPanel(glm::ivec2& pos, int w);
    void setupMutePanel(glm::ivec2& pos, int w);
    void setupVideoFxPanel(glm::ivec2& pos);
    void setupMidiPanel(glm::ivec2& pos, int w);
    void setupMidiCcPanel(glm::ivec2& pos, int w);
    void setuAlphaPanel(glm::ivec2& pos);
    void setupRetriggerPanel(glm::ivec2& pos);
    void setupBlendModePanel(glm::ivec2& pos);

    void addBlank(ofxDatGui* panel);

    std::unique_ptr<ofxDatGui> controlPanel_, playPanel_, mutePanel_,
                               videoFxPanel_, midiPanel_, midiCcPanel_,
                               alphaPanel_, retriggerPanel_, blendModePanel_;

    // This class does not own any of the following pointers, do not try to delete them.
    ofxDatGuiTextInput* sceneNameInput_;
    ofxDatGuiTextInput* masterAlphaInput_;
    ofxDatGuiTextInput* midiChannelInput_;
    std::vector<ofxDatGuiButton*> controlButtons_;
    std::array<ofxDatGuiButton*,    MAX_LAYERS> layerButtons_;
    std::array<ofxDatGuiTextInput*, MAX_LAYERS> layerMidiInputs_;
    std::array<ofxDatGuiTextInput*, MAX_LAYERS> layerCCInputs_;
    std::array<ofxDatGuiLabel*,     MAX_LAYERS> layerAlphaLabels_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerPlayToggles_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerMuteToggles_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerRetriggerToggles_;
    std::array<ofxDatGuiDropdown*,  MAX_LAYERS> blendModeDropdowns_;
    std::array<ofxDatGuiButton*,    static_cast<int>(Effect::Type::Count)> effectButtons_;
    std::array<ofxDatGuiTextInput*, static_cast<int>(Effect::Type::Count)> effectMidiInputs_;
    std::array<ofxDatGuiToggle*,    static_cast<int>(Effect::Type::Count)> effectPlayToggles_;
    std::array<ofxDatGuiToggle*,    static_cast<int>(Effect::Type::Count)> effectMuteToggles_;

    Scene* currentScene_;
    ShowDescription* show_;
    std::string configPath_;
};
