#include "ofApp.h"

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

    if (settings_.verbose)
        ofxMidiIn::listPorts();
    for (auto portNumber : settings_.midiPorts) {
        midiInputs_.push_back(std::make_unique<ofxMidiIn>());
        midiInputs_.back()->openPort(portNumber);
        midiInputs_.back()->addListener(this);
        midiInputs_.back()->setVerbose(settings_.verbose);
    }    


    if (!shader_.setupShaderFromFile(GL_COMPUTE_SHADER, "../../src/computeShader.glsl"))
        ofExit();
    if (!shader_.linkProgram())
        ofExit();

    layers_.push_back(std::make_unique<Layer>(0, "numbers.mp4"));
    layers_.push_back(std::make_unique<Layer>(1, "simpleRGB.mp4"));
    layers_[0]->addMidiNote(40);
    layers_[1]->addMidiNote(36);
    
    //for (auto& layer : layers_)
    //    layer->play();
   
    dst_.allocate(width_, height_, GL_RGBA8);
    dst_.bindAsImage(2, GL_WRITE_ONLY);
}

//--------------------------------------------------------------
void ofApp::update(){
    bool newFrame = false;
    for (auto& layer : layers_)
        newFrame |= layer->update();
    if (newFrame || shouldRedraw_) {
        auto alphas = { layers_[0]->getAlpha(), layers_[1]->getAlpha() };
        shader_.begin();
        shader_.setUniform1fv("alphas", alphas.begin(), alphas.size());
        shader_.dispatchCompute(width_ / 32, height_ / 32, 1);
        shader_.end();
        shouldRedraw_ = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    dst_.draw(0, 0);
}

void ofApp::exit() {
    for (auto& midiInput : midiInputs_) {
        midiInput->closePort();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key)
    {
    case '1':
        layers_[0]->playPause();
        break;
    case '2':
        layers_[1]->playPause();
        break;
    case OF_KEY_F11:
        ofGetCurrentWindow()->toggleFullscreen();
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

void ofApp::newMidiMessage(ofxMidiMessage & msg) {
    auto noteOn = msg.status == MIDI_NOTE_ON;
    auto noteOff = msg.status == MIDI_NOTE_OFF;
    int note = msg.pitch;

    if (!noteOn && !noteOff)
        return;

    for (auto& layer : layers_) {
        if (layer->containsMidiNote(note)) {
            if (noteOn)
                layer->play();
            else if (noteOff)
                layer->pause();
        }
    }

    shouldRedraw_ = true;
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
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
