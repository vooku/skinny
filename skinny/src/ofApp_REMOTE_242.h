#pragma once

#include "ofMain.h"
#include "ofxArgs.h"
#include "Show.h"
#include "meta.h"
#include "Gui.h"
#include "MidiController.h"

namespace skinny {

class ofApp : public ofBaseApp {
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

private:
    struct {
        bool cancelSetup = false;
        bool verbose     = false;
        bool console     = false;
    } settings_;

    [[deprecated("Console configuration is going to be removed soon.")]]
    static void usage();
    [[deprecated("Console configuration is going to be removed soon.")]]
    void parseArgs(ofxArgs* args);

    bool reload();

    std::shared_ptr<Show> show_;
    ShowDescription showDescription_;

    Gui gui_;
    MidiController midiController_;
};

} // namespace skinny
