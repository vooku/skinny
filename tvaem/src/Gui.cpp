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
    leftPanel_->addLabel("Video");
    leftPanel_->addBreak();
    for (int i = 0; i < Scene::maxLayers; ++i) {
        layerButtons_.push_back(leftPanel_->addButton({}));
        layerButtons_.back()->onButtonEvent(this, &Gui::onLayerButtonEvent);
    }
    leftPanel_->addBreak();
    leftPanel_->addLabel("Effect");
    leftPanel_->addBreak();
    for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
        effectButtons_.push_back(leftPanel_->addButton({}));
        effectButtons_.back()->onButtonEvent(this, &Gui::onEffectButtonEvent);
    }

    // Mid panel (MIDI)
    auto midPanelWidth = 3 * delta;
    midPanel_ = std::make_unique<ofxDatGui>(delta + layerButtons_[0]->getWidth(), 2 * delta);
    midPanel_->setWidth(midPanelWidth);
    midPanel_->addLabel("MIDI");
    midPanel_->addBreak();
    for (int i = 0; i < Scene::maxLayers + static_cast<int>(Effect::Type::Count) + 1; ++i) {
        if (i == Scene::maxLayers) {
            midPanel_->addBreak();
            addBlank(midPanel_.get());
            midPanel_->addBreak();
        }
        else {
            midiInputs_.push_back(midPanel_->addTextInput({}));
            midiInputs_.back()->setInputType(ofxDatGuiInputType::NUMERIC);
            midiInputs_.back()->onTextInputEvent(this, &Gui::onMidiInputEvent);
            midiInputs_.back()->setWidth(midPanelWidth, 0); // This doesn't seem to work right
        }        
    }

    // Right panel
    
}

void Gui::draw(const std::string& sceneName)
{
    ofBackground(bgColor);

    fonts.italic.drawString(status_.forward ? "Loading..." : sceneName, delta, delta);
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

void Gui::onLayerButtonEvent(ofxDatGuiButtonEvent e)
{
    printf("Layer button presed.\n");
    // TODO
}

void Gui::onEffectButtonEvent(ofxDatGuiButtonEvent e)
{
    printf("Effect button presed.\n");
    // TODO
}

void Gui::onOtherButtonEvent(ofxDatGuiButtonEvent e)
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
}

void Gui::onMidiInputEvent(ofxDatGuiTextInputEvent e)
{
    printf("MIDI: %s.\n", e.text);
    // TODO
}

void Gui::Gui::addBlank(ofxDatGui * panel)
{
    auto blank = panel->addButton({});
    blank->setEnabled(false);
    blank->setBackgroundColor(bgColor);
    blank->setStripeColor(bgColor);
}