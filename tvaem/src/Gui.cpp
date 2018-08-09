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

    for (int i = 0; i < layerToggles_.size(); ++i) {
        layerToggles_[i] = playPanel_->addToggle({});
        layerToggles_[i]->onToggleEvent(this, &Gui::onLayerPlayToggleEvent);
        layerToggles_[i]->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        layerToggles_[i]->setName(std::to_string(i));
    }

    playPanel_->addBreak();
    addBlank(playPanel_.get());
    playPanel_->addBreak();

    for (int i = 0; i < effectToggles_.size(); ++i) {
        effectToggles_[i] = playPanel_->addToggle({});
        effectToggles_[i]->onToggleEvent(this, &Gui::onEffectPlayToggleEvent);
        effectToggles_[i]->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        effectToggles_[i]->setName(std::to_string(i));
    }

    // Videos & FX panel
    videoFxPanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    videoFxPanel_->setTheme(&commonTheme_);
    videoFxPanel_->setWidth(14 * delta);
    xOffset += videoFxPanel_->getWidth();
    videoFxPanel_->addLabel("Video")->setTheme(&headerTheme_);
    videoFxPanel_->addBreak();

    for (int i = 0; i < layerButtons_.size(); ++i) {
        layerButtons_[i] = videoFxPanel_->addButton({});
        layerButtons_[i]->onButtonEvent(this, &Gui::onLayerButtonEvent);
        layerButtons_[i]->setName(std::to_string(i));
    }

    videoFxPanel_->addBreak();
    videoFxPanel_->addLabel("Effect");
    videoFxPanel_->addBreak();

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
    for (int i = 0; i < layerMidiInputs_.size(); ++i) {
        layerMidiInputs_[i] = midiPanel_->addTextInput({});
        layerMidiInputs_[i]->setName(std::to_string(i));
        layerMidiInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        layerMidiInputs_[i]->onTextInputEvent(this, &Gui::onLayerMidiInputEvent);
        layerMidiInputs_[i]->setWidth(midiPanelWidth, 0); // This doesn't seem to work right
    }

    midiPanel_->addBreak();
    addBlank(midiPanel_.get());
    midiPanel_->addBreak();

    for (int i = 0; i < effectMidiInputs_.size(); ++i) {
        effectMidiInputs_[i] = midiPanel_->addTextInput({});
        effectMidiInputs_[i]->setName(std::to_string(i));
        effectMidiInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        effectMidiInputs_[i]->onTextInputEvent(this, &Gui::onEffectMidiInputEvent);
        effectMidiInputs_[i]->setWidth(midiPanelWidth, 0); // This doesn't seem to work right
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

    for (int i = 0; i < blendModeDropdowns_.size(); ++i) {
        blendModeDropdowns_[i] = blendModePanel_->addDropdown("Select...", options);
        blendModeDropdowns_[i]->setName(std::to_string(i));
        blendModeDropdowns_[i]->select(static_cast<int>(Layer::BlendMode::Normal));
        blendModeDropdowns_[i]->onDropdownEvent(this, &Gui::onBlendModeDropdownEvent);
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

    // layers
    for (auto& toggle : layerToggles_) {
        toggle->setChecked(false);
    }

    auto& layers = currentScene_->getLayers();
    if (layerButtons_.size() != layers.size()) {
        ofLog(OF_LOG_WARNING, "Different number of layers and layer buttons");
    }
    else {
        for (int i = 0; i < MAX_LAYERS; ++i) {
            if (layers[i]) {
                layerButtons_[i]->setLabel(layers[i]->getName());
                auto& midiMap = layers[i]->getMapping();
                if (!midiMap.empty())
                    layerMidiInputs_[i]->setText(std::to_string(*midiMap.begin()));
                blendModeDropdowns_[i]->select(static_cast<int>(layers[i]->getBlendMode()));
            }
        }
    }

    // effects
    for (auto& toggle : effectToggles_) {
        toggle->setChecked(false);
    }

    auto& effects = currentScene_->getEffects();
    for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
        auto type = static_cast<Effect::Type>(i);
        effectButtons_[i]->setLabel(Effect::c_str(type));
        auto& midiMap = effects.at(type).getMapping();
        if (!midiMap.empty())
            effectMidiInputs_[i]->setText(std::to_string(*midiMap.begin()));
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
        auto& layerDescription = show_->scenes_[show_->currentIdx_].layers[idx];
        layerDescription.path = openFileResult.getPath();
        status_->reload = true;
    }
}

void Gui::onEffectButtonEvent(ofxDatGuiButtonEvent e)
{
    //printf("Effect button presed.\n");
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

void Gui::onLayerMidiInputEvent(ofxDatGuiTextInputEvent e)
{
    auto idx = std::stoi(e.target->getName());
    auto note = static_cast<midiNote>(std::stoi(e.text));
    show_->scenes_[show_->currentIdx_].layers[idx].midiMap = { note };
    if (currentScene_)
        currentScene_->layers_[idx]->setMapping({ note });
}

void Gui::onEffectMidiInputEvent(ofxDatGuiTextInputEvent e)
{
    auto idx = std::stoi(e.target->getName());
    auto note = static_cast<midiNote>(std::stoi(e.text));
    show_->scenes_[show_->currentIdx_].effects[idx].midiMap = { note };
    if (currentScene_)
        currentScene_->effects_[static_cast<Effect::Type>(idx)].setMapping({ note });
}

void Gui::onBlendModeDropdownEvent(ofxDatGuiDropdownEvent e)
{
    auto idx = std::stoi(e.target->getName());
    auto blendMode = static_cast<Layer::BlendMode>(e.child);
    auto& layerDescription = show_->scenes_[show_->currentIdx_].layers[idx];
    layerDescription.blendMode = blendMode;
    if (currentScene_)
        currentScene_->layers_[idx]->setBlendMode(blendMode);
}

void Gui::onLayerPlayToggleEvent(ofxDatGuiToggleEvent e)
{
    auto idx = std::stoi(e.target->getName());
    if (currentScene_)
        currentScene_->playPauseLayer(idx);
}
void Gui::onEffectPlayToggleEvent(ofxDatGuiToggleEvent e)
{
    auto type = static_cast<Effect::Type>(std::stoi(e.target->getName()));
    if (currentScene_)
        currentScene_->playPauseEffect(type);
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
