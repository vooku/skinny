#include "Base.h"
#include "Gui.h"
#include "ofApp.h"
#include "ofMain.h"
#include "../resource.h"

#ifdef TARGET_WIN32
extern "C" {
_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

//--------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  ofGLWindowSettings settings;

  settings.setGLVersion(4, 1);
  settings.setSize(skinny::MAIN_WINDOW_WIDTH, skinny::MAIN_WINDOW_HEIGHT);
  auto mainWindow = ofCreateWindow(settings);

  settings.setSize(skinny::GUI_WINDOW_WIDTH, skinny::GUI_WINDOW_HEIGHT);
  auto guiWindow = ofCreateWindow(settings);

  const auto mainWindowHwnd = mainWindow->getWin32Window();
  const auto guiWindowHwnd = guiWindow->getWin32Window();
  const auto icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  SendMessage(mainWindowHwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
  SendMessage(guiWindowHwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);

  auto mainApp = std::make_shared<skinny::ofApp>();
  auto guiApp = std::make_shared<skinny::Gui>();


  ofRunApp(guiWindow, guiApp); // run gui first so main can access it
  ofRunApp(mainWindow, mainApp);

  ofRunMainLoop();
}
