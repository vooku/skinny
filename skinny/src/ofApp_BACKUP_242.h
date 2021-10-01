#pragma once

#include "ofMain.h"
#include "ofxArgs.h"
#include "meta.h"
<<<<<<< Updated upstream
=======
#include "Gui.h"
#include "MidiController.h"
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
    std::vector<std::unique_ptr<ofxMidiIn>> midiInputs_;

=======
    std::shared_ptr<Show> show_;
    ShowDescription showDescription_;

    Gui gui_;
    MidiController midiController_;
>>>>>>> Stashed changes
};

} // namespace skinny
