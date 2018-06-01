#include "ofMain.h"
#include "ofApp.h"

int main(int argc, char *argv[]) {
    std::unique_ptr<ofxArgs> args = std::make_unique<ofxArgs>(argc, argv);

    ofGLWindowSettings settings;

    settings.setGLVersion(4, 3);
    settings.setSize(1920, 1080);
    auto mainWindow = ofCreateWindow(settings);

    settings.setSize(800, 600);
    auto guiWindow = ofCreateWindow(settings);
    
    shared_ptr<ofApp> mainApp(new ofApp(args.get()));
    mainApp->setupGui();
    ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);
    ofAddListener(guiWindow->events().exit, mainApp.get(), &ofApp::exitGui);

    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
}
