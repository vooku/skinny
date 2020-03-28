#include "ofMain.h"
#include "ofApp.h"

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int main(int argc, char *argv[]) {
    auto args = std::make_unique<ofxArgs>(argc, argv);

    ofGLWindowSettings settings;

    settings.setGLVersion(4, 3);
    settings.setSize(1920, 1080);
    auto mainWindow = ofCreateWindow(settings);

    settings.setSize(1250, 700);
    auto guiWindow = ofCreateWindow(settings);

    shared_ptr<skinny::ofApp> mainApp(new skinny::ofApp(args.get()));
    mainApp->setupGui();
    ofAddListener(guiWindow->events().draw, mainApp.get(), &skinny::ofApp::drawGui);
    ofAddListener(guiWindow->events().update, mainApp.get(), &skinny::ofApp::updateGui);
    ofAddListener(guiWindow->events().exit, mainApp.get(), &skinny::ofApp::exitGui);
    ofAddListener(guiWindow->events().keyReleased, mainApp.get(), &skinny::ofApp::keyReleasedGui);

    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
}
