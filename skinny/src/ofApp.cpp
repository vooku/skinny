#include <ctime>
#include "ofApp.h"
#include "Status.h"

namespace skinny {

//--------------------------------------------------------------
ofApp::ofApp(ofxArgs* args)
{
    parseArgs(args);
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

    setupMidi();

    getStatus().show() = make_shared<Show>(ofGetCurrentWindow()->getWidth(), ofGetCurrentWindow()->getHeight());
    getStatus().loadDir = LoadDir::Current;
    reload();
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
    ofSetWindowTitle(NAME);

    getStatus().gui().setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
    if (getStatus().exit) {
        ofExit();
        //return;
    }

    if (getStatus().loadDir != LoadDir::None) {
        reload();
    }

    getStatus().show()->update();

    if (ofGetFrameNum() % 300 == 0) {
        ofLog(OF_LOG_NOTICE, "fps: %f", ofGetFrameRate());
    }
}

//--------------------------------------------------------------
void ofApp::updateGui(ofEventArgs& args)
{
  getStatus().gui().update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(ofColor::black);
    getStatus().show()->draw();
}

//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs&) {

    getStatus().gui().draw();
}

//--------------------------------------------------------------
void ofApp::exit()
{
    for (auto& midiInput : midiInputs_) {
        midiInput->closePort();
    }
}

//--------------------------------------------------------------
void ofApp::exitGui(ofEventArgs&) {
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
void ofApp::newMidiMessage(ofxMidiMessage & msg)
{
    const auto& showDescription = getStatus().showDescription();
    if (msg.channel != showDescription.getMidiChannel()) {
        ofLog(OF_LOG_WARNING, "Received a MIDI message on an incorrect channel: %d %d %d.", msg.channel, msg.status, msg.pitch);
        return;
    }

    if (msg.status == MIDI_NOTE_ON && msg.pitch == showDescription.getSwitchNote()) {
        getStatus().loadDir = LoadDir::Forward;
    }
    else {
        auto activeMappables = getStatus().show()->newMidiMessage(msg);

        for (const auto& layer : activeMappables.layers) {
            getStatus().gui().setActiveLayer(layer.first, layer.second);
        }
        for (const auto& effect : activeMappables.effects) {
            getStatus().gui().setActiveEffect(effect.first, effect.second);
        }
    }
}

//--------------------------------------------------------------
void ofApp::usage()
{
    std::cout <<
        "Usage:\n"
        "    -h, --help, --usage    Print this message.\n"
        "    --midiport <number>    Try to open up a MIDI port <number> for input.\n"
        "    --console              Log to console instead to a file."
        "    -v, --verbose Use verbose mode."
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

    if (args->contains("--midiport")) {
        settings_.midiPorts.push_back(args->getInt("--midiport", 0));
    }
    else { // --midiports-all
        ofxMidiIn tmpMidiIn;
        for (auto i = 0; i < tmpMidiIn.getNumInPorts(); ++i)
            settings_.midiPorts.push_back(i);
    }

    settings_.verbose = args->contains("-v") || args->contains("--verbose");
    settings_.console = args->contains("--console");
}

//--------------------------------------------------------------
void ofApp::setupMidi()
{
    if (settings_.verbose) {
        ofxMidiIn tmpMidiIn;
        tmpMidiIn.listInPorts();
    }

    for (auto portNumber : settings_.midiPorts) {
        midiInputs_.push_back(std::make_unique<ofxMidiIn>());
        midiInputs_.back()->openPort(portNumber);
        midiInputs_.back()->addListener(this);
        midiInputs_.back()->setVerbose(/*settings_.verbose*/false);
    }
}

//--------------------------------------------------------------
bool ofApp::reload()
{
    auto& showDescription = getStatus().showDescription();
    if (showDescription.getSize() < 1) {
        ofLog(OF_LOG_ERROR, "Cannot load a scene from and empty show.");
        return false;
    }

    getStatus().gui().displayMessage("Loading...");

    const auto shifted = showDescription.shift(getStatus().loadDir, getStatus().jumpToIndex);
    if (!shifted && getStatus().loadDir != LoadDir::Current) {
        getStatus().gui().resetJumpToIndex();
        return false;
    }

    if (getStatus().show()->reload(showDescription)) {
        ofLog(OF_LOG_NOTICE, "Successfully loaded scene %s.", showDescription.currentScene().name.c_str());
        getStatus().gui().reload();
    }
    else {
        // TODO display in gui
        ofLog(OF_LOG_WARNING, "Scene %s encountered loading problems.", showDescription.currentScene().name.c_str());
    }

    getStatus().loadDir = LoadDir::None;
    return true;
}

} // namespace skinny
