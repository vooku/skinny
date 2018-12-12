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
    static constexpr const char * NAME = "Triggered Video & FX Mixer";

    explicit ofApp(ofxArgs* args);

    void setup() override;
    void setupGui();
    void update() override;
    void draw() override;
    void drawGui(ofEventArgs& args);
    void exit() override;
    void exitGui(ofEventArgs& args);

    void keyReleased(ofKeyEventArgs& key) override;
    void keyReleasedGui(ofKeyEventArgs& args);
    void newMidiMessage(ofxMidiMessage& msg) override;

private:
    struct {
        std::vector<unsigned int> midiPorts;
        bool cancelSetup = false;
        bool verbose     = false;
        bool console     = false;
        std::string cfgFile;
    } settings_;

    static void usage();
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
