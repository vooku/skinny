#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Scene.h"
#include "Status.h"

class Gui {
public:
    static const ofColor bgColor;

    Gui() = delete;
    Gui(Status& status) : status_(status) {}

    void setup();
    void draw(const std::string& sceneName);
    void reload(const Scene* currentScene);

    void onLayerButtonEvent(ofxDatGuiButtonEvent e);
    void onEffectButtonEvent(ofxDatGuiButtonEvent e);
    void onOtherButtonEvent(ofxDatGuiButtonEvent e);
    void onMidiInputEvent(ofxDatGuiTextInputEvent e);
    void onBlendModeDropdownEvent(ofxDatGuiDropdownEvent e);
    void onPlayToggleEvent(ofxDatGuiToggleEvent e);

private:
    static const int MAX_CHARS = 20;
    static const int delta = 25;

    class Theme : public ofxDatGuiTheme {
    public:
        Theme();
    } theme_;
    
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
    std::vector<ofxDatGuiToggle*> playPauseToggles_;

    ofDirectory dir_;

    Status& status_;
};
