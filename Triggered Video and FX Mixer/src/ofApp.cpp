#include "ofApp.h"
#include "ofxXmlSettings.h"

ofApp::ofApp(ofxArgs* args) {
    parseArgs(args);
}

void ofApp::setup() {
    if (settings_.cancelSetup) {
        ofExit();
        return;
    }

    ofLogToConsole(); // TODO log file
    ofSetLogLevel(settings_.verbose ? OF_LOG_VERBOSE : OF_LOG_NOTICE);
    ofSetFrameRate(30);
    ofBackground(ofColor::black);
    ofSetVerticalSync(true);

    int major = ofGetGLRenderer()->getGLVersionMajor();
    int minor = ofGetGLRenderer()->getGLVersionMinor();
    auto *vendor = (char*)glGetString(GL_VENDOR);
    auto *renderer = (char*)glGetString(GL_RENDERER);
    ofLog(OF_LOG_NOTICE, "Using OpenGL v%d.%d, GPU: %s %s.", major, minor, vendor, renderer);
    if (major < 4 && minor < 3) {
        ofLog(OF_LOG_FATAL_ERROR, "OpenGL version too old!", major, minor);
        ofExit();
        return;
    }

    width_ = ofGetCurrentWindow()->getWidth();
    height_ = ofGetCurrentWindow()->getHeight();
    shouldRedraw_ = false;

    setupMidi();

    if (!shader_.setupShaderFromFile(GL_COMPUTE_SHADER, "../../src/computeShader.glsl"))
        ofExit();
    if (!shader_.linkProgram())
        ofExit();

    if (!setupShow()) {
        ofLog(OF_LOG_FATAL_ERROR, "Could not setup show from configuration file.");
        ofExit();
    }

    dst_.allocate(width_, height_, GL_RGBA8);
    dst_.bindAsImage(7, GL_WRITE_ONLY);
}

void ofApp::setupGui() {
    auto cwd = ".";
    dir_.open(cwd);
    dir_.allowExt("mp4");
    dir_.allowExt("avi");
    dir_.listDir();
}

//--------------------------------------------------------------
void ofApp::update(){
    if (currentScene_->isFrameNew() || shouldRedraw_) {
        shader_.begin();
        currentScene_->setupUniforms(shader_);
        shader_.dispatchCompute(width_ / 32, height_ / 32, 1);
        shader_.end();
        shouldRedraw_ = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    dst_.draw(0, 0);
}

void ofApp::drawGui(ofEventArgs& args) {
    ofBackground(ofColor(45, 45, 48));
    
    for (int i = 0; i < dir_.size(); i++) {
        ofDrawBitmapString(dir_.getPath(i), 20, 20 + i * 20);
    }
}

void ofApp::exit() {
    for (auto& midiInput : midiInputs_) {
        midiInput->closePort();
    }
}

void ofApp::exitGui(ofEventArgs& args) {
    ofExit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // 0-9 in ascii
    if (key >= 0x30 && key < 0x3A) {
        // key to index 1->0, 0->9
        currentScene_->playPauseLayer((key - 0x30 + 9) % 10);
    }
    
    switch (key)
    {
    case OF_KEY_F11:
        ofGetCurrentWindow()->toggleFullscreen();
        break;
    case 'n':
        loadNext();
        break;
    }    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}


void ofApp::newMidiMessage(ofxMidiMessage & msg) {
    if (currentScene_) {
        currentScene_->newMidiMessage(msg);
        shouldRedraw_ = true;
    }
}

void ofApp::usage() const {
    std::cout <<
        "Usage:\n"
        "    -h, --help, --usage Print this message.\n"
        "    --list-midiports List available MIDI ports.\n"
        "    --midiport <number> Try to open up a MIDI port <number> for input as listed by --list-midiports\n"
        "    --midiports-all Open all available MIDI ports for input.\n"
        "    --config <file>  Load configuration from <file>.\n"
        "    -v, --verbose Use verbose mode"
        << std::endl;
}

void ofApp::parseArgs(ofxArgs* args) {
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
    settings_.configFileName = args->getString("--config", "");
}

void ofApp::setupMidi() {
    if (settings_.verbose)
        ofxMidiIn::listPorts();
    for (auto portNumber : settings_.midiPorts) {
        midiInputs_.push_back(std::make_unique<ofxMidiIn>());
        midiInputs_.back()->openPort(portNumber);
        midiInputs_.back()->addListener(this);
        midiInputs_.back()->setVerbose(/*settings_.verbose*/false);
    }
}

bool ofApp::setupShow() {
    if (settings_.configFileName != "") {
        if (!loadConfig()) {
            return false;
        }
    }

    if (show_.getSize() >= 1) {
        currentScene_ = std::make_unique<Scene>(show_.currentScene());
        currentScene_->bindTextures();
    }
    else {
        ofLog(OF_LOG_FATAL_ERROR, "Configuration file %s contains no scenes.", settings_.configFileName);
        return false;
    }

    if (show_.getSize() >= 2) {
        nextScene_ = std::make_unique<Scene>(show_.nextScene());
    }

    return true;
}

bool ofApp::loadConfig() {
    ofxXmlSettings config;
    if (!config.loadFile(settings_.configFileName))
        return false;

    config.pushTag("config");
    show_.fromXml(config);
    config.popTag(); // config

    return true;
}

bool ofApp::saveConfig() {  
    ofxXmlSettings config;
    config.addTag("head");
    config.pushTag("head");
    config.setValue("version", version);
    config.popTag(); // head

    config.addTag("config");
    config.pushTag("config");
    show_.toXml(config);
    config.popTag(); // config
    return config.saveFile(settings_.configFileName);
}

void ofApp::loadNext() {
    if (show_.getSize() <= 1) {
        ofLog(OF_LOG_ERROR, "Cannot load next scene, %d is too few.", show_.getSize());
        return;
    }

    shader_.begin();
    if (nextScene_) {
        ++show_;
        currentScene_.reset(nextScene_.release());
        nextScene_ = std::make_unique<Scene>(show_.nextScene());
    }
    else {
        ++show_;
        currentScene_.reset(new Scene(show_.currentScene()));
        nextScene_ = std::make_unique<Scene>(show_.nextScene());
    }
    currentScene_->bindTextures();
    shader_.end();
}