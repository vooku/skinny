#include "ofMain.h"
#include "ofApp.h"

int main(int argc, char *argv[]) {
    std::unique_ptr<ofxArgs> args = std::make_unique<ofxArgs>(argc, argv);
    ofGLWindowSettings settings;
    settings.setGLVersion(4, 3);
    settings.setSize(1920, 1080);
    ofCreateWindow(settings);
    ofRunApp(new ofApp(args.get()));
}
