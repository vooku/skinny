#include <ctime>
#include "ofApp.h"
#include "ofxXmlSettings.h"
#include "shader.h"

ofApp::ofApp(ofxArgs* args) : 
    switchNote_(MappableDescription::invalid_midi),
    gui_(&status_, &show_),
    currentScene_(std::make_unique<Scene>())
{
    parseArgs(args);
}

void ofApp::setup()
{
    if (settings_.cancelSetup) {
        status_.exit = true;
        return;
    }

    ofSetWindowTitle(name);

    if (settings_.console)
        ofLogToConsole();
    else 
        ofLogToFile("tvaem.log", true);
    ofSetLogLevel(settings_.verbose ? OF_LOG_VERBOSE : OF_LOG_NOTICE);
    ofSetFrameRate(30);
    ofBackground(ofColor::black);
    ofSetVerticalSync(true);

    int major = ofGetGLRenderer()->getGLversionMajor();
    int minor = ofGetGLRenderer()->getGLversionMinor();
    auto *vendor = (char*)glGetString(GL_VENDOR);
    auto *renderer = (char*)glGetString(GL_RENDERER);
    ofLog(OF_LOG_NOTICE, "Using OpenGL v%d.%d, GPU: %s %s.", major, minor, vendor, renderer);
    if (major < 4 && minor < 3) {
        ofLog(OF_LOG_FATAL_ERROR, "OpenGL version too old!", major, minor);
        status_.exit = true;
        return;
    }

    width_ = ofGetCurrentWindow()->getWidth();
    height_ = ofGetCurrentWindow()->getHeight();

    setupMidi();

    if (!shader_.setupShaderFromSource(GL_COMPUTE_SHADER, computeShader)) {
        ofLog(OF_LOG_FATAL_ERROR, "Could not load shader.");
        status_.exit = true;
        return;
    }
    if (!shader_.linkProgram()) {
        ofLog(OF_LOG_FATAL_ERROR, "Could not link shader.");
        status_.exit = true;
        return;
    }

    if (!setupShow()) {
        ofLog(OF_LOG_FATAL_ERROR, "Could not setup show from configuration file.");
        status_.exit = true;
        return;
    }

    dst_.allocate(width_, height_, GL_RGBA8);
    dst_.bindAsImage(7, GL_WRITE_ONLY);
}

void ofApp::setupGui()
{
    ofSetWindowTitle(name);

    gui_.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
    if (status_.exit) {
        ofExit();
        //return;
    }

    if (status_.forward) {
        //drawGui(ofEventArgs{ });
        reload(LoadDir::Forward);
        status_.forward = false;
    }
    else if (status_.backward) {
        //drawGui(ofEventArgs{});
        reload(LoadDir::Backward);
        status_.backward = false;
    }
    else if (status_.reload) {
        //drawGui(ofEventArgs{});
        reload(LoadDir::Current);
        status_.reload = false;
    }

    if (!currentScene_)
        return;

    if (currentScene_->isFrameNew() || status_.redraw) {
        shader_.begin();
        currentScene_->setupUniforms(shader_);
        shader_.dispatchCompute(width_ / 32, height_ / 32, 1);
        shader_.end();
        status_.redraw = currentScene_->hasActiveFX();
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    dst_.draw(0, 0);
}


void ofApp::drawGui(ofEventArgs& args) {
    gui_.draw();
}

void ofApp::exit() 
{
    for (auto& midiInput : midiInputs_) {
        midiInput->closePort();
    }
}

void ofApp::exitGui(ofEventArgs& args) {
    status_.exit = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    // 0-9 in ascii
    //if (key >= 0x30 && key < 0x3A) {
    //    // key to index 1->0, 0->9
    //    currentScene_->playPauseLayer((key - 0x30 + 9) % 10);
    //}

    switch (key) {
    case OF_KEY_F11:
        ofGetCurrentWindow()->toggleFullscreen();
        break;
    case 'n':
        status_.forward = true;
        break;
    case 'b':
        status_.backward = true;
        break;
    }
}

void ofApp::keyReleasedGui(ofKeyEventArgs & args)
{
    keyReleased(args.codepoint);
}

void ofApp::newMidiMessage(ofxMidiMessage & msg)
{
    if (msg.status == MIDI_NOTE_ON && msg.pitch == switchNote_)
        status_.forward = true;
    else if (currentScene_) {
        auto foundMappables = currentScene_->newMidiMessage(msg);
        for (const auto& layer : foundMappables.layers) {
            gui_.setActive(layer.first, layer.second);
        }
        for (const auto& effect : foundMappables.effects) {
            gui_.setActive(effect.first, effect.second);
        }

        status_.redraw = true;
    }
}

void ofApp::usage() const
{
    std::cout <<
        "Usage:\n"
        "    -h, --help, --usage Print this message.\n"
        "    --list-midiports List available MIDI ports.\n"
        "    --midiport <number> Try to open up a MIDI port <number> for input as listed by --list-midiports.\n"
        "    --midiports-all Open all available MIDI ports for input.\n"
        "    --config <file>  Load configuration from <file>.\n"
        "    --console Log to console."
        "    -v, --verbose Use verbose mode."
        << std::endl;
}

void ofApp::parseArgs(ofxArgs* args)
{
    if (args->contains("-h") || args->contains("-help") || args->contains("-usage")) {
        usage();
        settings_.cancelSetup = true;
        return;
    }

    if (args->contains("--list-midiports")) {
        ofxMidiIn::listPorts();
        settings_.cancelSetup = true;
        return;
    }

    if (args->contains("--midiports-all")) {
        for (int i = 0; i < ofxMidiIn::getNumPorts(); ++i)
            settings_.midiPorts.push_back(i);
    }
    else {
        settings_.midiPorts.push_back(args->getInt("--midiport", 0));
    }

    settings_.verbose = args->contains("-v") || args->contains("--verbose");
    settings_.console = args->contains("--console");
    settings_.cfgFile = args->getString("--config", "");
}

void ofApp::setupMidi() 
{
    if (settings_.verbose)
        ofxMidiIn::listPorts();
    for (auto portNumber : settings_.midiPorts) {
        midiInputs_.push_back(std::make_unique<ofxMidiIn>());
        midiInputs_.back()->openPort(portNumber);
        midiInputs_.back()->addListener(this);
        midiInputs_.back()->setVerbose(/*settings_.verbose*/false);
    }
}

bool ofApp::setupShow()
{
    if (settings_.cfgFile == "" || !loadConfig()) {
        show_.appendScene();
    }

    if (!reload(LoadDir::Current)) {
        ofLog(OF_LOG_FATAL_ERROR, "Configuration file \"%s\" contains no scenes.", settings_.cfgFile.c_str());
        return false;
    }

    return true;
}

bool ofApp::loadConfig()
{
    ofxXmlSettings config;
    if (!config.loadFile(settings_.cfgFile)) {
        ofLog(OF_LOG_WARNING, "Cannot load config file %s, creating default scene instead.", settings_.cfgFile.c_str());
        return false;
    }

    config.pushTag("head");
    switchNote_ = config.getValue("switchNote", MappableDescription::invalid_midi);
    config.popTag(); // head
    
    config.pushTag("show");
    show_.fromXml(config);
    config.popTag(); // show
    
    return true;
}

bool ofApp::saveConfig()
{  
    ofxXmlSettings config;
    config.addTag("head");
    config.pushTag("head");
    config.setValue("version", version);
    config.setValue("switchNote", switchNote_);
    config.popTag(); // head

    config.addTag("show");
    config.pushTag("show");
    show_.toXml(config);
    config.popTag(); // show
    return config.saveFile(settings_.cfgFile);
}

bool ofApp::reload(LoadDir dir)
{
    if (show_.getSize() < 1) {
        ofLog(OF_LOG_ERROR, "Cannot load next scene, %d is too few.", show_.getSize());
        return false;
    }

    switch (dir)
    {
    case ofApp::LoadDir::Forward:
        ++show_;
        break;
    case ofApp::LoadDir::Backward:
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
        ofLog(OF_LOG_NOTICE, "Succesfully loaded scene %s.", currentScene_->getName().c_str());
        gui_.reload(currentScene_.get());
    }
    else {
        // TODO display in gui
        ofLog(OF_LOG_WARNING, "Scene %s encountered loading problems.", currentScene_->getName().c_str());
    }

    status_.redraw = true;

    return true;
}
