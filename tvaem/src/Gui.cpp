#include "Gui.h"

const ofColor Gui::bgColor = { 45, 45, 48 };

void Gui::setup()
{
    auto cwd = ".";
    dir_.open(cwd);
    dir_.allowExt("mp4");
    dir_.allowExt("avi");
    dir_.listDir();

    fonts.regular.load("fonts/IBMPlexSans-Regular.ttf", fonts.size, true, false);
    fonts.italic.load("fonts/IBMPlexSerif-Italic.ttf", fonts.size, true, false);

    // Left panel (videos)
    leftPanel_ = std::make_unique<ofxDatGui>(delta, 2 * delta);
    leftPanel_->addLabel("Videos");
    for (int i = 0; i < Scene::maxLayers; ++i) {
        layerButtons_.push_back(leftPanel_->addButton({}));
        layerButtons_.back()->onButtonEvent(this, &Gui::onButtonEvent);
    }
    //addBlank(leftPanel.get());
    leftPanel_->addLabel("Effects");
    for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
        effectButtons_.push_back(leftPanel_->addButton({}));
        layerButtons_.back()->onButtonEvent(this, &Gui::onButtonEvent);
    }

    // Mid panel (MIDI)
    midPanel_ = std::make_unique<ofxDatGui>(delta + layerButtons_[0]->getWidth(), 2 * delta);
    for (int i = 0; i < Scene::maxLayers; ++i) {
        midiInputs_.push_back(midPanel_->addTextInput({}));
        midiInputs_.back()->setInputType(ofxDatGuiInputType::NUMERIC);
        midiInputs_.back()->setLabel("MIDI");
    }
    addBlank(midPanel_.get());
    for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
        midiInputs_.push_back(midPanel_->addTextInput({}));
        midiInputs_.back()->setInputType(ofxDatGuiInputType::NUMERIC);
        midiInputs_.back()->setLabel("MIDI");
    }
}

void Gui::draw()
{
    ofBackground(bgColor);

    //fonts.italic.drawString(status_.forward ? "Loading..." : currentScene_->getName(), delta, delta);
    fonts.italic.drawString("fps: " + std::to_string(ofGetFrameRate()), delta, ofGetHeight() - delta);

    if (leftPanel_) leftPanel_->draw();
    if (midPanel_) midPanel_->draw();
    if (rightPanel_) rightPanel_->draw();
}

void Gui::reload(const Scene * currentScene)
{
    if (layerButtons_.size() == Scene::maxLayers) {
        for (int i = 0; i < Scene::maxLayers; ++i) {
            if (i < currentScene->layerNames.size())
                layerButtons_[i]->setLabel(currentScene->layerNames[i]);
            else
                layerButtons_[i]->setLabel("Click to load a video"); // TODO different style
        }
    }
    if (effectButtons_.size() == static_cast<int>(Effect::Type::Count)) {
        for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
            effectButtons_[i]->setLabel(currentScene->effectNames[i]);
        }
    }
}

void Gui::onButtonEvent(ofxDatGuiButtonEvent e)
{
    if (e.target->getName() == "Next scene") {
        status_.forward = true;
    }
    else if (e.target->getName() == "Previous scene") {
        status_.backward = true;
    }
    else {
        ofLog(OF_LOG_WARNING, "Unassigned button \"%s\" pressed.", e.target->getName().c_str());
    }
    // TODO
}

void Gui::Gui::addBlank(ofxDatGui * panel)
{
    auto blank = panel->addButton({});
    blank->setEnabled(false);
    blank->setBackgroundColor(bgColor);
    blank->setStripeColor(bgColor);
}