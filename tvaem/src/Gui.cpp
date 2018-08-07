#include "Gui.h"
#include <algorithm>

const ofColor Gui::bgColor = { 45, 45, 48 };

void Gui::setup()
{
    auto cwd = ".";
    dir_.open(cwd);
    dir_.allowExt("mp4");
    dir_.allowExt("avi");
    dir_.listDir();

    fonts.regular.load("fonts/IBMPlexSans-Regular.ttf", fonts.sizeRegular, true, false);
    fonts.italic.load("fonts/IBMPlexSerif-Italic.ttf", fonts.sizeItalic, true, false);

    auto xOffset = delta;
    auto yOffset = 2 * delta;
    
    // Play & pause panel
    playPanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    playPanel_->setTheme(&commonTheme_);
    auto playPanelWidth = 2 * delta;
    playPanel_->setWidth(playPanelWidth);
    xOffset += playPanel_->getWidth();
    playPanel_->addLabel("Play");
    playPanel_->addBreak();
    for (int i = 0; i < Scene::maxLayers + static_cast<int>(Effect::Type::Count) + 1; ++i) {
        if (i == Scene::maxLayers) {
            playPanel_->addBreak();
            addBlank(playPanel_.get());
            playPanel_->addBreak();
        }
        else {
            playPauseToggles_.push_back(playPanel_->addToggle({}));
            playPauseToggles_.back()->onToggleEvent(this, &Gui::onPlayToggleEvent);
            playPauseToggles_.back()->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        }
    }

    // Videos & FX panel
    videoFxPanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    videoFxPanel_->setTheme(&commonTheme_);
    xOffset += videoFxPanel_->getWidth();
    videoFxPanel_->addLabel("Video")->setTheme(&headerTheme_);
    videoFxPanel_->addBreak();
    for (int i = 0; i < Scene::maxLayers; ++i) {
        layerButtons_.push_back(videoFxPanel_->addButton({}));
        layerButtons_.back()->onButtonEvent(this, &Gui::onLayerButtonEvent);
    }
    videoFxPanel_->addBreak();
    videoFxPanel_->addLabel("Effect");
    videoFxPanel_->addBreak();
    for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
        effectButtons_.push_back(videoFxPanel_->addButton({}));
        effectButtons_.back()->onButtonEvent(this, &Gui::onEffectButtonEvent);
    }

    // MIDI panel
    auto midiPanelWidth = 3 * delta;
    midiPanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    midiPanel_->setTheme(&commonTheme_);
    midiPanel_->setWidth(midiPanelWidth);
    xOffset += midiPanel_->getWidth();
    midiPanel_->addLabel("MIDI");
    midiPanel_->addBreak();
    for (int i = 0; i < Scene::maxLayers + static_cast<int>(Effect::Type::Count) + 1; ++i) {
        if (i == Scene::maxLayers) {
            midiPanel_->addBreak();
            addBlank(midiPanel_.get());
            midiPanel_->addBreak();
        }
        else {
            midiInputs_.push_back(midiPanel_->addTextInput({}));
            midiInputs_.back()->setInputType(ofxDatGuiInputType::NUMERIC);
            midiInputs_.back()->onTextInputEvent(this, &Gui::onMidiInputEvent);
            midiInputs_.back()->setWidth(midiPanelWidth, 0); // This doesn't seem to work right
        }        
    }

    // Blending modes panel
    blendModePanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    blendModePanel_->setTheme(&commonTheme_);
    xOffset += blendModePanel_->getWidth();
    blendModePanel_->addLabel("Blending Mode");
    blendModePanel_->addBreak();
    std::vector<string> options;
    for (int i = static_cast<int>(Layer::BlendMode::Normal); i < static_cast<int>(Layer::BlendMode::Count); ++i)
        options.push_back(Layer::c_str(static_cast<Layer::BlendMode>(i)));
    for (int i = 0; i < Scene::maxLayers; ++i) {
        blendModeDropdowns_.push_back(blendModePanel_->addDropdown("Select...", options));
        blendModeDropdowns_.back()->select(static_cast<int>(Layer::BlendMode::Normal));
        blendModeDropdowns_.back()->onDropdownEvent(this, &Gui::onBlendModeDropdownEvent);
    }
    blendModePanel_->addBreak();
}

void Gui::draw(const std::string& sceneName)
{
    ofBackground(bgColor);

    fonts.italic.drawString(status_.forward ? "Loading..." : sceneName, delta, delta);
    fonts.italic.drawString("fps: " + std::to_string(ofGetFrameRate()), delta, ofGetHeight() - delta);

    if (playPanel_) playPanel_->draw();
    if (videoFxPanel_) videoFxPanel_->draw();
    if (midiPanel_) midiPanel_->draw();
    if (blendModePanel_) blendModePanel_->draw();
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
    printf("MIDI: %s.\n", e.text.c_str());
    // TODO
}

void Gui::onBlendModeDropdownEvent(ofxDatGuiDropdownEvent e)
{
    printf("Blending Mode: %s\n", Layer::c_str(static_cast<Layer::BlendMode>(e.child)));
    // TODO
}

void Gui::onPlayToggleEvent(ofxDatGuiToggleEvent e)
{
    printf("Play pause %s\n", e.checked > 0 ? "checked" : "unchecked");
    // TODO
}

void Gui::Gui::addBlank(ofxDatGui * panel)
{
    auto blank = panel->addButton({});
    blank->setEnabled(false);
    blank->setBackgroundColor(bgColor);
    //blank->setStripeColor(bgColor);
}

Gui::CommonTheme::CommonTheme() :
    ofxDatGuiTheme(false) 
{
    layout.upperCaseLabels = false;
    stripe.visible = false;
    font.size = Gui::Fonts::sizeRegular;
    font.file = "fonts/IBMPlexSans-Regular.ttf";
    
    init();
}

Gui::HeaderTheme::HeaderTheme() :
    ofxDatGuiTheme(false)
{
    layout.upperCaseLabels = false;
    stripe.visible = false;
    font.size = Gui::Fonts::sizeItalic;
    font.file = "fonts/IBMPlexSerif-Italic.ttf";

    init();
}
