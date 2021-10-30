#include "ofMain.h"
#include "ofApp.h"
#include "Gui.h"

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

//--------------------------------------------------------------
int main(int argc, char *argv[]) {
    auto args = std::make_unique<ofxArgs>(argc, argv);

    ofGLWindowSettings settings;

    settings.setGLVersion(4, 3);
    settings.setSize(1920, 1080);
    auto mainWindow = ofCreateWindow(settings);

    settings.setSize(1312, 800);
    auto guiWindow = ofCreateWindow(settings);

    auto mainApp = std::make_shared<skinny::ofApp>(args.get());
    auto guiApp = std::make_shared<skinny::Gui>();
        
    ofRunApp(guiWindow, guiApp); // run gui first so main can access it
    ofRunApp(mainWindow, mainApp);

    ofRunMainLoop();
}
