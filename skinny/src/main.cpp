#include "ofMain.h"
#include "ofApp.h"
#include "Base.h"
#include "Gui.h"

#ifdef TARGET_WIN32
extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

//--------------------------------------------------------------
int main(int argc, char *argv[]) {
    auto args = std::make_unique<ofxArgs>(argc, argv);

    ofGLWindowSettings settings;

    settings.setGLVersion(4, 1);
    settings.setSize(skinny::MAIN_WINDOW_WIDTH, skinny::MAIN_WINDOW_HEIGHT);
    auto mainWindow = ofCreateWindow(settings);

    settings.setSize(skinny::GUI_WINDOW_WIDTH, skinny::GUI_WINDOW_HEIGHT);
    auto guiWindow = ofCreateWindow(settings);

    auto mainApp = std::make_shared<skinny::ofApp>(args.get());
    auto guiApp = std::make_shared<skinny::Gui>();
        
    ofRunApp(guiWindow, guiApp); // run gui first so main can access it
    ofRunApp(mainWindow, mainApp);

    ofRunMainLoop();
}
