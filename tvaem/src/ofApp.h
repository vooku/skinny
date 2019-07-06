#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxArgs.h"
#include "Show.h"
#include "meta.h"
#include "Gui.h"

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
    } settings_;

    [[deprecated("Console configuration is going to be removed soon.")]]
    static void usage();
    [[deprecated("Console configuration is going to be removed soon.")]]
    void parseArgs(ofxArgs* args);
    void setupMidi();

    enum class LoadDir {
        Current, Forward, Backward
    };

    bool reload(LoadDir dir);

    std::shared_ptr<Show> show_;
    ShowDescription showDescription_;

    std::vector<std::unique_ptr<ofxMidiIn>> midiInputs_;

    Gui gui_;
};
