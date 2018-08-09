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
    auto yOffset = delta;
    
    // Control panel
    controlPanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    controlPanel_->setTheme(&headerTheme_);
    xOffset += controlPanel_->getWidth() + 2 * delta;
    
    sceneNameInput_ = controlPanel_->addTextInput("Scene");
    sceneNameInput_->setText(currentScene_ ? currentScene_->getName() : "Enter scene name");
    sceneNameInput_->onTextInputEvent(this, &Gui::onSceneNameInput);

    controlButtons_.push_back(controlPanel_->addButton("Next scene"));
    controlButtons_.push_back(controlPanel_->addButton("Previous scene"));
    controlButtons_.push_back(controlPanel_->addButton("Append scene"));
    controlPanel_->addBreak();
    controlButtons_.push_back(controlPanel_->addButton("Save config"));
    controlButtons_.push_back(controlPanel_->addButton("Load config"));
    for (auto& btn : controlButtons_)
        btn->onButtonEvent(this, &Gui::onOtherButton);

    // Play & pause panel
    playPanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    playPanel_->setTheme(&commonTheme_);
    auto playPanelWidth = 2 * delta;
    playPanel_->setWidth(playPanelWidth);
    xOffset += playPanel_->getWidth();
    playPanel_->addLabel("Play");
    playPanel_->addBreak();

    for (int i = 0; i < layerPlayToggles_.size(); ++i) {
        layerPlayToggles_[i] = playPanel_->addToggle({});
        layerPlayToggles_[i]->onToggleEvent(this, &Gui::onLayerPlayToggle);
        layerPlayToggles_[i]->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        layerPlayToggles_[i]->setName(std::to_string(i));
    }

    playPanel_->addBreak();
    addBlank(playPanel_.get());
    playPanel_->addBreak();

    for (int i = 0; i < effectPlayToggles_.size(); ++i) {
        effectPlayToggles_[i] = playPanel_->addToggle({});
        effectPlayToggles_[i]->onToggleEvent(this, &Gui::onEffectPlayToggle);
        effectPlayToggles_[i]->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        effectPlayToggles_[i]->setName(std::to_string(i));
    }

    // Mute panel
    mutePanel_ = std::make_unique<ofxDatGui>(xOffset, yOffset);
    mutePanel_->setTheme(&commonTheme_);
    mutePanel_->setWidth(playPanelWidth);
    xOffset += mutePanel_->getWidth();
    mutePanel_->addLabel("Mute");
    mutePanel_->addBreak();

    for (int i = 0; i < layerMuteToggles_.size(); ++i) {
        layerMuteToggles_[i] = mutePanel_->addToggle({});
        layerMuteToggles_[i]->onToggleEvent(this, &Gui::onLayerMuteToggle);
        layerMuteToggles_[i]->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        layerMuteToggles_[i]->setName(std::to_string(i));
    }

    mutePanel_->addBreak();
    addBlank(mutePanel_.get());
    mutePanel_->addBreak();

    for (int i = 0; i < effectMuteToggles_.size(); ++i) {
        effectMuteToggles_[i] = mutePanel_->addToggle({});
        effectMuteToggles_[i]->onToggleEvent(this, &Gui::onEffectMuteToggle);
        effectMuteToggles_[i]->setWidth(playPanelWidth, 0); // This doesn't seem to work right
        effectMuteToggles_[i]->setName(std::to_string(i));
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
        layerButtons_[i]->onButtonEvent(this, &Gui::onLayerButton);
        layerButtons_[i]->setName(std::to_string(i));
    }

    videoFxPanel_->addBreak();
    videoFxPanel_->addLabel("Effect");
    videoFxPanel_->addBreak();

    for (auto& button : effectButtons_) {
        button = videoFxPanel_->addButton({});
        button->onButtonEvent(this, &Gui::onEffectButton);
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
        layerMidiInputs_[i]->onTextInputEvent(this, &Gui::onLayerMidiInput);
        layerMidiInputs_[i]->setWidth(midiPanelWidth, 0); // This doesn't seem to work right
    }

    midiPanel_->addBreak();
    addBlank(midiPanel_.get());
    midiPanel_->addBreak();

    for (int i = 0; i < effectMidiInputs_.size(); ++i) {
        effectMidiInputs_[i] = midiPanel_->addTextInput({});
        effectMidiInputs_[i]->setName(std::to_string(i));
        effectMidiInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        effectMidiInputs_[i]->onTextInputEvent(this, &Gui::onEffectMidiInput);
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
        blendModeDropdowns_[i]->onDropdownEvent(this, &Gui::onBlendModeDropdown);
    }
    blendModePanel_->addBreak();
}

void Gui::draw()
{
    ofBackground(bgColor);

    if (status_->forward || status_->backward || status_->reload)
        fonts.italic.drawString("Loading...", 2 * delta, controlPanel_->getHeight() + 3 * delta);
    fonts.italic.drawString("fps: " + std::to_string(ofGetFrameRate()), delta, ofGetHeight() - delta);

    if (controlPanel_) controlPanel_->draw();
    if (playPanel_) playPanel_->draw();
    if (mutePanel_) mutePanel_->draw();
    if (videoFxPanel_) videoFxPanel_->draw();
    if (midiPanel_) midiPanel_->draw();
    if (blendModePanel_) blendModePanel_->draw();
}

void Gui::reload(Scene* newScene)
{
    currentScene_ = newScene;
    sceneNameInput_->setText(currentScene_->getName());

    // layers
    for (auto& toggle : layerPlayToggles_) {
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
            else {
                layerButtons_[i]->setLabel("Click to load a video");
                layerMidiInputs_[i]->setText("");
                blendModeDropdowns_[i]->select(0);
            }
        }
    }

    // effects
    for (auto& toggle : effectPlayToggles_) {
        toggle->setChecked(false);
    }

    auto& effects = currentScene_->getEffects();
    for (int i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
        auto type = static_cast<Effect::Type>(i);
        effectButtons_[i]->setLabel(Effect::c_str(type));
        auto& midiMap = effects.at(type).getMapping();
        effectMidiInputs_[i]->setText(midiMap.empty() ? "" : std::to_string(*midiMap.begin()));
    }

    draw();
}

void Gui::setActive(int layerId, bool active)
{
    if (layerId < layerPlayToggles_.size()) {
        layerPlayToggles_[layerId]->setChecked(active);
    }
    else {
        ofLog(OF_LOG_WARNING, "Trying to activate layer #%d out of bounds", layerId);
    }
}

void Gui::setActive(Effect::Type type, bool active)
{
    if (static_cast<int>(type) < effectPlayToggles_.size()) {
        effectPlayToggles_.at(static_cast<int>(type))->setChecked(active);
    }
    else {
        ofLog(OF_LOG_WARNING, "Trying to activate effect #%d out of bounds", static_cast<int>(type));
    }
}

void Gui::onLayerButton(ofxDatGuiButtonEvent e)
{
    auto openFileResult = ofSystemLoadDialog("Select a video");
    if (openFileResult.bSuccess) {
        auto idx = std::stoi(e.target->getName());
        auto& layerDescription = show_->scenes_[show_->currentIdx_].layers[idx];
        if (layerDescription.valid)
            layerDescription.path = openFileResult.getPath();
        else
            layerDescription = { static_cast<unsigned int>(idx), openFileResult.getPath() };
        status_->reload = true;
    }
}

void Gui::onEffectButton(ofxDatGuiButtonEvent e)
{
    //printf("Effect button presed.\n");
    // nothing
}

void Gui::onOtherButton(ofxDatGuiButtonEvent e)
{
    const auto name = e.target->getName();
    if (name == "Next scene") {
        status_->forward = true;
    }
    else if (name == "Previous scene") {
        status_->backward = true;
    }
    else if (name == "Append scene") {
        show_->scenes_.push_back({ "New scene" });
    }
    else {
        ofLog(OF_LOG_WARNING, "Unassigned button \"%s\" pressed.", name.c_str());
    }
}

void Gui::onLayerMidiInput(ofxDatGuiTextInputEvent e)
{
    auto idx = std::stoi(e.target->getName());
    auto note = static_cast<midiNote>(std::stoi(e.text));
    show_->scenes_[show_->currentIdx_].layers[idx].midiMap = { note };
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setMapping({ note });
}

void Gui::onEffectMidiInput(ofxDatGuiTextInputEvent e)
{
    auto idx = std::stoi(e.target->getName());
    auto note = static_cast<midiNote>(std::stoi(e.text));
    show_->scenes_[show_->currentIdx_].effects[idx].midiMap = { note };
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->effects_[static_cast<Effect::Type>(idx)].setMapping({ note });
}

void Gui::onSceneNameInput(ofxDatGuiTextInputEvent e)
{
    show_->scenes_[show_->currentIdx_].name = e.text;
    if (currentScene_)
        currentScene_->name_ = e.text;
}

void Gui::onBlendModeDropdown(ofxDatGuiDropdownEvent e)
{
    auto idx = std::stoi(e.target->getName());
    auto blendMode = static_cast<Layer::BlendMode>(e.child);
    show_->scenes_[show_->currentIdx_].layers[idx].blendMode = blendMode;
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setBlendMode(blendMode);
}

void Gui::onLayerPlayToggle(ofxDatGuiToggleEvent e)
{    
    auto idx = std::stoi(e.target->getName());
    if (currentScene_) {
        currentScene_->playPauseLayer(idx);
        status_->redraw = true;
    }
}

void Gui::onEffectPlayToggle(ofxDatGuiToggleEvent e)
{
    
auto type = static_cast<Effect::Type>(std::stoi(e.target->getName()));
    if (currentScene_)
        currentScene_->playPauseEffect(type);
}

void Gui::onLayerMuteToggle(ofxDatGuiToggleEvent e)
{
    bool mute = e.checked;
    auto idx = std::stoi(e.target->getName());
    if (currentScene_)
        currentScene_->layers_[idx]->setMute(mute);
    if (mute)
        layerPlayToggles_[idx]->setChecked(false);
}

void Gui::onEffectMuteToggle(ofxDatGuiToggleEvent e)
{
    bool mute = e.checked;
    auto idx = std::stoi(e.target->getName());
    auto type = static_cast<Effect::Type>(idx);
    if (currentScene_)
        currentScene_->effects_[type].setMute(mute);
    if (mute)
        effectPlayToggles_[idx]->setChecked(false);
}

void Gui::addBlank(ofxDatGui * panel)
{
    auto blank = panel->addButton({});
    blank->setEnabled(false);
    blank->setBackgroundColor(bgColor);
    //blank->setStripeColor(bgColor);
}

Gui::CommonTheme::CommonTheme() :
    ofxDatGuiTheme(false) 
{
    layout.height *= 1.25;
    layout.upperCaseLabels = false;
    layout.textInput.forceUpperCase = false;
    color.textInput.text = ofColor::antiqueWhite;
    color.label = ofColor::antiqueWhite;
    stripe.visible = false;
    font.size = Gui::Fonts::sizeRegular;
    font.file = "fonts/IBMPlexSans-Regular.ttf";
    
    init();
}

Gui::HeaderTheme::HeaderTheme() :
    CommonTheme()
{
    layout.height *= 1.4;
    font.size = Gui::Fonts::sizeItalic;
    font.file = "fonts/IBMPlexSerif-Italic.ttf";

    init();
}
