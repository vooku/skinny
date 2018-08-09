#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Scene.h"
#include "Status.h"

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
    void onMidiInputEvent(ofxDatGuiTextInputEvent e);
    void onBlendModeDropdownEvent(ofxDatGuiDropdownEvent e);
    void onPlayLayerToggleEvent(ofxDatGuiToggleEvent e);
    void onPlayEffectToggleEvent(ofxDatGuiToggleEvent e);

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
    std::vector<ofxDatGuiButton*> layerButtons_, effectButtons_;
    std::vector<ofxDatGuiTextInput*> midiInputs_;
    std::vector<ofxDatGuiDropdown*> blendModeDropdowns_;
    std::vector<ofxDatGuiToggle*> layerToggles_, effectToggles_;

    Status* status_; //!< No ownership, do not delete here!
    Scene* currentScene_; //!< No ownership, do not delete here!
    ShowDescription* show_; //!< No ownership, do not delete here!
};
