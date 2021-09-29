#include <ctime>
#include "ofApp.h"
#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
ofApp::ofApp(ofxArgs* args)
{
    parseArgs(args);

    getStatus().showDescription = showDescription_ = make_shared<ShowDescription>();
    getStatus().midi = midiController_ = make_shared<MidiController>();
}

//--------------------------------------------------------------
void ofApp::setup()
{
    if (settings_.cancelSetup) {
        getStatus().exit = true;
        return;
    }

    ofSetWindowTitle(NAME);
#ifdef NDEBUG
    ofSetEscapeQuitsApp(false);
#endif

    if (settings_.console)
        ofLogToConsole();
    else
        ofLogToFile("skinny.log", true);
    ofSetLogLevel(settings_.verbose ? OF_LOG_VERBOSE : OF_LOG_NOTICE);
    ofSetFrameRate(30);
    ofBackground(ofColor::black);
    ofSetVerticalSync(true);

    ofLog(OF_LOG_NOTICE, "%s %s", NAME, VERSION);
    ofLog(OF_LOG_NOTICE, "Designed by %s.", AUTHOR);

    const auto major = ofGetGLRenderer()->getGLVersionMajor();
    const auto minor = ofGetGLRenderer()->getGLVersionMinor();
    const auto vendor = glGetString(GL_VENDOR);
    const auto renderer = glGetString(GL_RENDERER);
    ofLog(OF_LOG_NOTICE, "Using OpenGL v%d.%d, GPU: %s %s.", major, minor, vendor, renderer);
    if (major < 4 && minor < 3) {
        ofLog(OF_LOG_FATAL_ERROR, "OpenGL version too old!", major, minor);
        getStatus().exit = true;
        return;
    }

    getStatus().show = show_ = make_shared<Show>(ofGetCurrentWindow()->getWidth(), ofGetCurrentWindow()->getHeight());
    show_->init();

    midiController_->setup(settings_.verbose);

    getStatus().loadDir = LoadDir::Current;

    reload();
}

//--------------------------------------------------------------
void ofApp::update()
{
    if (getStatus().exit) {
        ofExit();
    }

    if (getStatus().loadDir != LoadDir::None) {
        reload();
    }

    getStatus().show->update();

    if (ofGetFrameNum() % 300 == 0) {
        ofLog(OF_LOG_NOTICE, "fps: %f", ofGetFrameRate());
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(ofColor::black);
    getStatus().show->draw();
}


//--------------------------------------------------------------
void ofApp::exit()
{
  getStatus().midi->exit();
  getStatus().show->done();
  getStatus().exit = true; 
}

//--------------------------------------------------------------
void ofApp::keyReleased(ofKeyEventArgs& key)
{
    switch(key.key) {
    case OF_KEY_F11:
        ofGetCurrentWindow()->toggleFullscreen();
        break;
    default:
        // nothing
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleasedGui(ofKeyEventArgs & args)
{
    keyReleased(args);
}

//--------------------------------------------------------------
void ofApp::usage()
{
    std::cout <<
        "Usage:\n"
        "    -h, --help, --usage    Print this message.\n"
        "    --console              Log to console instead to a file."
        "    -v, --verbose          Use verbose mode."
        << std::endl;
}

//--------------------------------------------------------------
void ofApp::parseArgs(ofxArgs* args)
{
    if (args->contains("-h") || args->contains("--help") || args->contains("--usage")) {
        usage();
        settings_.cancelSetup = true;
        return;
    }

    settings_.verbose = args->contains("-v") || args->contains("--verbose");
    settings_.console = args->contains("--console");
}

//--------------------------------------------------------------
bool ofApp::reload()
{
    auto& showDescription = *getStatus().showDescription;
    if (showDescription.getSize() < 1) {
        ofLog(OF_LOG_ERROR, "Cannot load a scene from and empty show.");
        return false;
    }

    getStatus().gui->displayMessage("Loading...");

    const auto shifted = showDescription.shift(getStatus().loadDir, getStatus().jumpToIndex);
    if (!shifted && getStatus().loadDir != LoadDir::Current) {
        getStatus().gui->resetJumpToIndex();
        return false;
    }

    if (getStatus().show->reload(showDescription)) {
        ofLog(OF_LOG_NOTICE, "Successfully loaded scene %s.", showDescription.currentScene().name.c_str());
        getStatus().gui->reload();
    }
    else {
        // TODO display in gui
        ofLog(OF_LOG_WARNING, "Scene %s encountered loading problems.", showDescription.currentScene().name.c_str());
    }

    getStatus().loadDir = LoadDir::None;
    return true;
}

} // namespace skinny
