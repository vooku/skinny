#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Scene.h"
#include "Status.h"
#include <array>

class Gui {
public:
    static const ofColor bgColor;

    Gui() = delete;
    Gui(Status* status, ShowDescription* show);

    void setup();
    void draw();
    void reload(Scene* newScene);
    void setActive(int layerId, bool active);
    void setActive(Effect::Type type, bool active);

    void onLayerButton(ofxDatGuiButtonEvent e);
    void onEffectButton(ofxDatGuiButtonEvent e);
    void onOtherButton(ofxDatGuiButtonEvent e);

    void onLayerMidiInput(ofxDatGuiTextInputEvent e);
    void onEffectMidiInput(ofxDatGuiTextInputEvent e);
    void onSceneNameInput(ofxDatGuiTextInputEvent e);

    void onBlendModeDropdown(ofxDatGuiDropdownEvent e);

    void onLayerPlayToggle(ofxDatGuiToggleEvent e);
    void onEffectPlayToggle(ofxDatGuiToggleEvent e);
    void onLayerMuteToggle(ofxDatGuiToggleEvent e);
    void onEffectMuteToggle(ofxDatGuiToggleEvent e);

private:
    static const int MAX_CHARS = 20;
    static const int delta = 25;

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
    } fonts;
    
    void addBlank(ofxDatGui* panel);

    std::unique_ptr<ofxDatGui> controlPanel_, playPanel_, mutePanel_,
                               videoFxPanel_, midiPanel_, blendModePanel_;

    // This class does not own any of the following pointers, do not try to delete them.
    ofxDatGuiTextInput* sceneNameInput_;
    std::vector<ofxDatGuiButton*> controlButtons_;
    std::array<ofxDatGuiButton*,    MAX_LAYERS> layerButtons_;
    std::array<ofxDatGuiTextInput*, MAX_LAYERS> layerMidiInputs_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerPlayToggles_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerMuteToggles_;
    std::array<ofxDatGuiDropdown*,  MAX_LAYERS> blendModeDropdowns_;
    std::array<ofxDatGuiButton*,    static_cast<int>(Effect::Type::Count)> effectButtons_;
    std::array<ofxDatGuiTextInput*, static_cast<int>(Effect::Type::Count)> effectMidiInputs_;
    std::array<ofxDatGuiToggle*,    static_cast<int>(Effect::Type::Count)> effectPlayToggles_;
    std::array<ofxDatGuiToggle*,    static_cast<int>(Effect::Type::Count)> effectMuteToggles_;
    
    Status* status_;
    Scene* currentScene_;
    ShowDescription* show_;
};
