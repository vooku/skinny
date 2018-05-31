#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxArgs.h"
#include "Layer.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
public:
    ofApp(ofxArgs* args);

    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void newMidiMessage(ofxMidiMessage& msg);

private:
    struct Settings {
        Settings() : cancelSetup(false) { }

        std::vector<unsigned int> midiPorts;
        bool cancelSetup, verbose;

    } settings_;

    void usage() const;
    void parseArgs(ofxArgs* args);

    std::vector<std::unique_ptr<Layer>> layers_;
    ofShader shader_;
    ofTexture dst_;
    int width_, height_;
    bool shouldRedraw_;
    
    std::vector<std::unique_ptr<ofxMidiIn>> midiInputs_;
    ofxMidiMessage midiMessage_;
};
