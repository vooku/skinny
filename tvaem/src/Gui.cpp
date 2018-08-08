#include "Gui.h"
#include <algorithm>

const ofColor Gui::bgColor = { 45, 45, 48 };

Gui::Gui(Status* status, ShowDescription* show) :
    status_(status),
    show_(show),
    currentScene_(nullptr)
{
}

void Gui::setup()
{
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

    layerToggles_.resize(Scene::maxLayers);
    for (int i = 0; i < Scene::maxLayers; ++i) {
        layerToggles_[i] = playPanel_->addToggle({});
        layerToggles_[i]->onToggleEvent(this, &Gui::onPlayToggleEvent);
        layerToggles_[i]->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        layerToggles_[i]->setName("layer" + std::to_string(i));
    }

    playPanel_->addBreak();
    addBlank(playPanel_.get());
    playPanel_->addBreak();

    effectToggles_.resize(static_cast<int>(Effect::Type::Count));
    for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
        effectToggles_[i] = playPanel_->addToggle({});
        effectToggles_[i]->onToggleEvent(this, &Gui::onPlayToggleEvent);
        effectToggles_[i]->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        effectToggles_[i]->setName("effect" + std::to_string(i));
    }

    // Videos & FX panel
    videoFxPanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    videoFxPanel_->setTheme(&commonTheme_);
    videoFxPanel_->setWidth(14 * delta);
    xOffset += videoFxPanel_->getWidth();
    videoFxPanel_->addLabel("Video")->setTheme(&headerTheme_);
    videoFxPanel_->addBreak();

    layerButtons_.resize(Scene::maxLayers);
    for (int i = 0; i < layerButtons_.size(); ++i) {
        layerButtons_[i] = videoFxPanel_->addButton({});
        layerButtons_[i]->onButtonEvent(this, &Gui::onLayerButtonEvent);
        layerButtons_[i]->setName(std::to_string(i));
    }

    videoFxPanel_->addBreak();
    videoFxPanel_->addLabel("Effect");
    videoFxPanel_->addBreak();

    effectButtons_.resize(static_cast<int>(Effect::Type::Count));
    for (auto& button : effectButtons_) {
        button = videoFxPanel_->addButton({});
        button->onButtonEvent(this, &Gui::onEffectButtonEvent);
        button->setEnabled(false);
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

void Gui::draw()
{
    ofBackground(bgColor);

    fonts.italic.drawString(status_->forward || status_->backward ? "Loading..." : currentScene_->getName(), delta, delta);
    fonts.italic.drawString("fps: " + std::to_string(ofGetFrameRate()), delta, ofGetHeight() - delta);

    if (playPanel_) playPanel_->draw();
    if (videoFxPanel_) videoFxPanel_->draw();
    if (midiPanel_) midiPanel_->draw();
    if (blendModePanel_) blendModePanel_->draw();
}

void Gui::reload(Scene* newScene)
{
    currentScene_ = newScene;

    for (auto& toggle : layerToggles_) {
        toggle->setChecked(false);
    }
    for (auto& toggle : effectToggles_) {
        toggle->setChecked(false);
    }

    if (layerButtons_.size() == Scene::maxLayers) {
        for (int i = 0; i < Scene::maxLayers; ++i) {
            if (i < currentScene_->getLayerNames().size())
                layerButtons_[i]->setLabel(currentScene_->getLayerNames()[i]);
            else
                layerButtons_[i]->setLabel("Click to load a video"); // TODO different style
        }
    }

    if (effectButtons_.size() == static_cast<int>(Effect::Type::Count)) {
        for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
            effectButtons_[i]->setLabel(currentScene_->getEffectNames()[i]);
        }
    }
}

void Gui::setActive(int layerId, bool active)
{
    if (layerId < layerToggles_.size()) {
        layerToggles_.at(layerId)->setChecked(active);
    }
    else {
        ofLog(OF_LOG_WARNING, "Trying to activate layer #%d out of bounds", layerId);
    }
}

void Gui::setActive(Effect::Type type, bool active)
{
    if (static_cast<int>(type) < effectToggles_.size()) {
        effectToggles_.at(static_cast<int>(type))->setChecked(active);
    }
    else {
        ofLog(OF_LOG_WARNING, "Trying to activate effect #%d out of bounds", static_cast<int>(type));
    }
}

void Gui::onLayerButtonEvent(ofxDatGuiButtonEvent e)
{
    auto openFileResult = ofSystemLoadDialog("Select a video");
    if (openFileResult.bSuccess) {
        auto idx = std::stoi(e.target->getName());
        if (idx < show_->scenes_.size()) {
            show_->scenes_[show_->currentIdx_].layers[idx].path = openFileResult.getPath();
            status_->reload = true;
        }
    }
    else {
        ofSystemAlertDialog("Cannot open " + openFileResult.getPath() + ".");
    }
}

void Gui::onEffectButtonEvent(ofxDatGuiButtonEvent e)
{
    printf("Effect button presed.\n");
    // nothing
}

void Gui::onOtherButtonEvent(ofxDatGuiButtonEvent e)
{
    if (e.target->getName() == "Next scene") {
        status_->forward = true;
    }
    else if (e.target->getName() == "Previous scene") {
        status_->backward = true;
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
    const auto& name = e.target->getName();
    if (name.substr(0, 5).compare("layer") == 0) {
        int layerId = std::stoi(name.substr(5));
        if (currentScene_)
            currentScene_->playPauseLayer(layerId);
    }
    else if (name.substr(0, 6).compare("effect") == 0) {
        auto type = static_cast<Effect::Type>(std::stoi(name.substr(6)));
        if (currentScene_)
            currentScene_->playPauseEffect(type);
    }
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
