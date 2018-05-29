#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {   
    ofLogToConsole(); // TODO decide
    ofSetLogLevel(OF_LOG_VERBOSE);
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

    if (!shader_.setupShaderFromFile(GL_COMPUTE_SHADER, "../../src/computeShader.glsl"))
        ofExit();
    if (!shader_.linkProgram())
        ofExit();

    layers_.push_back(std::make_unique<Layer>(0, "numbers.mp4"));
    layers_.push_back(std::make_unique<Layer>(1, "simpleRGB.mp4"));
    for (auto& layer : layers_)
        layer->play();
   
    dst_.allocate(width_, height_, GL_RGBA8);
    dst_.bindAsImage(2, GL_WRITE_ONLY);
}

//--------------------------------------------------------------
void ofApp::update(){
    bool newFrame = false;
    for (auto& layer : layers_)
        newFrame |= layer->update();
    if (newFrame) {
        auto alphas = { layers_[0]->getAlpha(), layers_[1]->getAlpha() };
        shader_.begin();
        shader_.setUniform1fv("alphas", alphas.begin(), alphas.size());
        shader_.dispatchCompute(width_ / 32, height_ / 32, 1);
        shader_.end();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    dst_.draw(0, 0);
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

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
