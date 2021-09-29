#pragma once

#include "ofMain.h"
#include "ofxArgs.h"
#include "meta.h"
#include "Show.h"
#include "MidiController.h"

namespace skinny {

class ofApp : public ofBaseApp {
public:
    explicit ofApp(ofxArgs* args);

    void setup() override;
    void update() override;
    void draw() override;
    void exit() override;

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
    std::shared_ptr<ShowDescription> showDescription_;
    std::shared_ptr<MidiController> midiController_;
};

} // namespace skinny
