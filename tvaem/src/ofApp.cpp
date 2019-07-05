#include <ctime>
#include "ofApp.h"
#include "Status.h"

ofApp::ofApp(ofxArgs* args) :
    currentScene_(std::make_unique<Scene>()),
    gui_(&show_)
{
    parseArgs(args);
}

void ofApp::setup()
{
    if (settings_.cancelSetup) {
        Status::instance().exit = true;
        return;
    }

    ofSetWindowTitle(NAME);
#ifdef NDEBUG
    ofSetEscapeQuitsApp(false);
#endif

    if (settings_.console)
        ofLogToConsole();
    else
        ofLogToFile("tvaem.log", true);
    ofSetLogLevel(settings_.verbose ? OF_LOG_VERBOSE : OF_LOG_NOTICE);
    ofSetFrameRate(30);
    ofBackground(ofColor::black);
    ofSetVerticalSync(true);

    ofLog(OF_LOG_NOTICE, "TRIGERRED VIDEO & FX MIXER %s", VERSION);
    ofLog(OF_LOG_NOTICE, "Designed by Vadim Vooku Petrov.");

    const auto major = ofGetGLRenderer()->getGLversionMajor();
    const auto minor = ofGetGLRenderer()->getGLversionMinor();
    const auto vendor = glGetString(GL_VENDOR);
    const auto renderer = glGetString(GL_RENDERER);
    ofLog(OF_LOG_NOTICE, "Using OpenGL v%d.%d, GPU: %s %s.", major, minor, vendor, renderer);
    if (major < 4 && minor < 3) {
        ofLog(OF_LOG_FATAL_ERROR, "OpenGL version too old!", major, minor);
        Status::instance().exit = true;
        return;
    }

    width_ = ofGetCurrentWindow()->getWidth();
    height_ = ofGetCurrentWindow()->getHeight();

    setupMidi();

#ifndef NDEBUG
    if (!shader_.setupShaderFromFile(GL_COMPUTE_SHADER, "../../src/shader.comp")) {
#else
    if (!shader_.setupShaderFromFile(GL_COMPUTE_SHADER, "shader.comp")) {
#endif
        ofLog(OF_LOG_FATAL_ERROR, "Could not load shader.");
        Status::instance().exit = true;
        return;
    }
    if (!shader_.linkProgram()) {
        ofLog(OF_LOG_FATAL_ERROR, "Could not link shader.");
        Status::instance().exit = true;
        return;
    }

    dst_.allocate(width_, height_, GL_RGBA8);
    dst_.bindAsImage(7, GL_WRITE_ONLY);

    reload(LoadDir::Current);
}

void ofApp::setupGui()
{
    ofSetWindowTitle(NAME);

    gui_.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
    if (Status::instance().exit) {
        ofExit();
        //return;
    }

    if (Status::instance().forward) {
        //drawGui(ofEventArgs{ });
        reload(LoadDir::Forward);
        Status::instance().forward = false;
    }
    else if (Status::instance().backward) {
        //drawGui(ofEventArgs{});
        reload(LoadDir::Backward);
        Status::instance().backward = false;
    }
    else if (Status::instance().reload) {
        //drawGui(ofEventArgs{});
        reload(LoadDir::Current);
        Status::instance().reload = false;
    }

    if (!currentScene_)
        return;

    gui_.update();

    if (currentScene_->isFrameNew() || Status::instance().redraw) {
        shader_.begin();
        currentScene_->setupUniforms(shader_);
        shader_.dispatchCompute(width_ / 32, height_ / 32, 1);
        shader_.end();
        Status::instance().redraw = currentScene_->hasActiveFX();
    }

    if (ofGetFrameNum() % 300 == 0) {
        ofLog(OF_LOG_NOTICE, "fps: %f", ofGetFrameRate());
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(ofColor::black);
    dst_.draw(0, 0);
}


void ofApp::drawGui(ofEventArgs&) {

    gui_.draw();
}

void ofApp::exit()
{
    for (auto& midiInput : midiInputs_) {
        midiInput->closePort();
    }
}

void ofApp::exitGui(ofEventArgs&) {
    Status::instance().exit = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(ofKeyEventArgs& key)
{
    // 0-9 in ascii
    //if (key >= 0x30 && key < 0x3A) {
    //    // key to index 1->0, 0->9
    //    currentScene_->playPauseLayer((key - 0x30 + 9) % 10);
    //}

    switch(key.key) {
    case OF_KEY_F11:
        ofGetCurrentWindow()->toggleFullscreen();
        break;
    default:
        break;
    }
}

void ofApp::keyReleasedGui(ofKeyEventArgs & args)
{
    keyReleased(args);
}

void ofApp::newMidiMessage(ofxMidiMessage & msg)
{
    if (msg.channel != show_.getMidiChannel()) {
        ofLog(OF_LOG_WARNING, "Received a MIDI message on an incorrect channel: %d %d %d.", msg.channel, msg.status, msg.pitch);
        return;
    }

    if (msg.status == MIDI_NOTE_ON && msg.pitch == show_.getSwitchNote())
        Status::instance().forward = true;
    else if (currentScene_) {
        auto foundMappables = currentScene_->newMidiMessage(msg);
        for (const auto& layer : foundMappables.layers) {
            gui_.setActive(layer.first, layer.second);
        }
        for (const auto& effect : foundMappables.effects) {
            gui_.setActive(effect.first, effect.second);
        }

        Status::instance().redraw = true;
    }
}

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
        for (auto i = 0; i < ofxMidiIn::getNumPorts(); ++i)
            settings_.midiPorts.push_back(i);
    }

    settings_.verbose = args->contains("-v") || args->contains("--verbose");
    settings_.console = args->contains("--console");
}

void ofApp::setupMidi()
{
    if (settings_.verbose) {
        ofxMidiIn::listPorts();
    }

    for (auto portNumber : settings_.midiPorts) {
        midiInputs_.push_back(std::make_unique<ofxMidiIn>());
        midiInputs_.back()->openPort(portNumber);
        midiInputs_.back()->addListener(this);
        midiInputs_.back()->setVerbose(/*settings_.verbose*/false);
    }
}

bool ofApp::reload(LoadDir dir)
{
    if (show_.getSize() < 1) {
        ofLog(OF_LOG_ERROR, "Cannot load next scene, %d is too few.", show_.getSize());
        return false;
    }

    gui_.displayMessage("Loading...");

    switch (dir)
    {
    case LoadDir::Forward:
        ++show_;
        break;
    case LoadDir::Backward:
        --show_;
        break;
    default:
        // do nothing
        break;
    }

    shader_.begin();
    currentScene_->reload(show_.currentScene());
    currentScene_->bindTextures();
    shader_.end();

    if (currentScene_->isValid()) {
        ofLog(OF_LOG_NOTICE, "Successfully loaded scene %s.", currentScene_->getName().c_str());
        gui_.reload(currentScene_.get());
    }
    else {
        // TODO display in gui
        ofLog(OF_LOG_WARNING, "Scene %s encountered loading problems.", currentScene_->getName().c_str());
    }

    Status::instance().redraw = true;

    return true;
}
