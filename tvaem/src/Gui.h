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
    void draw();
    void reload(const Scene* currentScene);

    void onButtonEvent(ofxDatGuiButtonEvent e);

private:
    static const int MAX_CHARS = 20;
    static const int delta = 25;
    
    struct {
        static const int size = 14;
        ofTrueTypeFont regular, italic;
    } fonts;
    
    void addBlank(ofxDatGui* panel);

    std::unique_ptr<ofxDatGui> leftPanel_, midPanel_, rightPanel_;
    std::vector<ofxDatGuiButton*> layerButtons_, effectButtons_;
    std::vector<ofxDatGuiTextInput*> midiInputs_;
    
    ofDirectory dir_;

    Status& status_;
};
