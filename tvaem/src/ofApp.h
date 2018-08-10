#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxArgs.h"
#include "Scene.h"
#include "meta.h"
#include "Gui.h"
#include "Status.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
public:
    static constexpr const char * name = "Triggered Video & FX Mixer";
    
    ofApp(ofxArgs* args);

    void setup();
    void setupGui();
    void update();
    void draw();
    void drawGui(ofEventArgs& args);
    void exit();
    void exitGui(ofEventArgs& args);
    
    void keyReleased(ofKeyEventArgs& key);
    void keyReleasedGui(ofKeyEventArgs& args);
    void newMidiMessage(ofxMidiMessage& msg);

private:
    struct {
        std::vector<unsigned int> midiPorts;
        bool cancelSetup = false;
        bool verbose     = false;
        bool console     = false;
        std::string cfgFile;
    } settings_;

    void usage() const;
    void parseArgs(ofxArgs* args);
    void setupMidi();
    bool setupShow();

    enum class LoadDir {
        Current, Forward, Backward
    };
    bool reload(LoadDir dir);

    Status status_;

    ShowDescription show_;
    std::unique_ptr<Scene> currentScene_;
    std::vector<std::unique_ptr<ofxMidiIn>> midiInputs_;

    ofShader shader_;
    ofTexture dst_;
    int width_, height_;

    Gui gui_;
};
