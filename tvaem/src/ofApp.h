#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxArgs.h"
#include "ofxDatGui.h"
#include "Scene.h"
#include "meta.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
public:
    static constexpr const char * VERSION = "0.2.0-alpha";
    static constexpr const char * NAME = "Triggered Video & FX Mixer";

    ofApp(ofxArgs* args);

    void setup();
    void setupGui();
    void update();
    void draw();
    void drawGui(ofEventArgs& args);
    void exit();
    void exitGui(ofEventArgs& args);
    
    void keyPressed(int key) { }
    void keyReleased(int key);
    void keyReleasedGui(ofKeyEventArgs& args);
    void mouseMoved(int x, int y) { }
    void mouseDragged(int x, int y, int button) { }
    void mousePressed(int x, int y, int button) { }
    void mouseReleased(int x, int y, int button) { }
    void mouseEntered(int x, int y) { }
    void mouseExited(int x, int y) { }
    void windowResized(int w, int h) { }
    void dragEvent(ofDragInfo dragInfo) { }
    void gotMessage(ofMessage msg) { }
    void newMidiMessage(ofxMidiMessage& msg);
    void onButtonEvent(ofxDatGuiButtonEvent e);

private:
    static const int MAX_CHARS = 20;

    struct Settings {
        Settings() : cancelSetup(false) { }

        std::vector<unsigned int> midiPorts;
        bool cancelSetup, verbose, console;
        std::string cfgFile;
    } settings_;

    struct {
        bool redraw = false; //!< The canvas should be redrawn.
        bool exit = false;   //!< The app should exit.
        bool forward = false; //!< Next scene should be loaded.
        bool backward = false; //!< Previous scene should be loaded.
    } status_;

    void usage() const;
    void parseArgs(ofxArgs* args);
    void setupMidi();
    bool setupShow();
    bool loadConfig();
    bool saveConfig();
    bool loadNext();

    ShowDescription show_;

    std::unique_ptr<Scene> currentScene_;

    ofShader shader_;
    ofTexture dst_;
    int width_, height_;
        
    std::vector<std::unique_ptr<ofxMidiIn>> midiInputs_;
    uint8_t switchNote_;

    std::unique_ptr<ofxDatGui> gui_;
    struct {
        static const int size = 12;
        ofTrueTypeFont regular, italic;
    } fonts_;
    ofDirectory dir_;
};
