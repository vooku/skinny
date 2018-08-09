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

    void onLayerButtonEvent(ofxDatGuiButtonEvent e);
    void onEffectButtonEvent(ofxDatGuiButtonEvent e);
    void onOtherButtonEvent(ofxDatGuiButtonEvent e);
    void onLayerMidiInputEvent(ofxDatGuiTextInputEvent e);
    void onEffectMidiInputEvent(ofxDatGuiTextInputEvent e);
    void onBlendModeDropdownEvent(ofxDatGuiDropdownEvent e);
    void onLayerPlayToggleEvent(ofxDatGuiToggleEvent e);
    void onEffectPlayToggleEvent(ofxDatGuiToggleEvent e);

private:
    static const int MAX_CHARS = 20;
    static const int delta = 25;

    class CommonTheme : public ofxDatGuiTheme {
    public:
        CommonTheme();
    } commonTheme_;

    class HeaderTheme : public ofxDatGuiTheme {
    public:
        HeaderTheme();
    } headerTheme_;
    
    struct Fonts {
        static const int sizeRegular = 12;
        static const int sizeItalic = 14;
        ofTrueTypeFont regular, italic;
    } fonts;
    
    void addBlank(ofxDatGui* panel);

    std::unique_ptr<ofxDatGui> playPanel_, videoFxPanel_, midiPanel_, blendModePanel_;
    std::array<ofxDatGuiButton*,    MAX_LAYERS> layerButtons_;
    std::array<ofxDatGuiTextInput*, MAX_LAYERS> layerMidiInputs_;
    std::array<ofxDatGuiDropdown*,  MAX_LAYERS> blendModeDropdowns_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerToggles_;
    std::array<ofxDatGuiButton*,    static_cast<int>(Effect::Type::Count)> effectButtons_;
    std::array<ofxDatGuiTextInput*, static_cast<int>(Effect::Type::Count)> effectMidiInputs_;
    std::array<ofxDatGuiToggle*,    static_cast<int>(Effect::Type::Count)> effectToggles_;

    Status* status_; //!< No ownership, do not delete here!
    Scene* currentScene_; //!< No ownership, do not delete here!
    ShowDescription* show_; //!< No ownership, do not delete here!
};
