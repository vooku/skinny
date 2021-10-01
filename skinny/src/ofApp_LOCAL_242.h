#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxArgs.h"
#include "meta.h"

namespace skinny {

class ofApp : public ofBaseApp, public ofxMidiListener {
public:
    explicit ofApp(ofxArgs* args);

    void setup() override;
    void setupGui();
    void update() override;
    void updateGui(ofEventArgs& args);
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

    bool reload();

    std::vector<std::unique_ptr<ofxMidiIn>> midiInputs_;

};

} // namespace skinny
