#include "Gui.h"
#include "Status.h"

const ofColor Gui::BACKGROUND_COLOR = { 45, 45, 48 };

Gui::Gui(ShowDescription* show) :
    currentScene_(nullptr),
    show_(show)
{
}

void Gui::setup()
{
    fonts_.regular.load("fonts/IBMPlexSans-Regular.ttf", fonts_.sizeRegular, true, false);
    fonts_.italic.load("fonts/IBMPlexSerif-Italic.ttf", fonts_.sizeItalic, true, false);

    glm::ivec2 pos{ DELTA, DELTA };
    const auto toggleWidth = 2 * DELTA;
    const auto midiInWidth = 3 * DELTA;

    setupControlPanel(pos);
    setupPlayPanel(pos, toggleWidth);
    setupMutePanel(pos, toggleWidth);
    setupVideoFxPanel(pos);
    setupMidiPanel(pos, midiInWidth);
    setupMidiCcPanel(pos, midiInWidth);
    setuAlphaPanel(pos);
    setupBlendModePanel(pos);
}

void Gui::draw() const
{
    ofBackground(BACKGROUND_COLOR);

    if (Status::instance().forward || Status::instance().backward || Status::instance().reload)
        fonts_.italic.drawString("Loading...", 2 * DELTA, controlPanel_->getHeight() + 3 * DELTA);

    if (controlPanel_) controlPanel_->draw();
    //if (playPanel_) playPanel_->draw();
    //if (mutePanel_) mutePanel_->draw();
    //if (videoFxPanel_) videoFxPanel_->draw();
    //if (midiPanel_) midiPanel_->draw();
    //if (midiCcPanel_) midiCcPanel_->draw();
    //if (blendModePanel_) blendModePanel_->draw();
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
    assert(layerButtons_.size() == layers.size());

    for (auto i = 0; i < MAX_LAYERS; ++i) {
        if (layers[i]) {
            layerButtons_[i]->setLabel(layers[i]->getName());
            auto& midiMap = layers[i]->getMapping();
            if (!midiMap.empty())
                layerMidiInputs_[i]->setText(std::to_string(*midiMap.begin()));
            layerCCInputs_[i]->setText(std::to_string(layers[i]->getAlphaControl()));
            layerAlphaLabels_[i]->setLabel(std::to_string(static_cast<int>(layers[i]->getAlpha() * 127)));
            blendModeDropdowns_[i]->select(static_cast<int>(layers[i]->getBlendMode()));
        }
        else {
            layerButtons_[i]->setLabel("Click to load a video");
            layerMidiInputs_[i]->setText("");
            layerCCInputs_[i]->setText("");
            blendModeDropdowns_[i]->select(0);
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

void Gui::update()
{
    auto& layers = currentScene_->getLayers();
    for (auto i = 0; i < MAX_LAYERS; ++i) {
        if (layers[i]) {
            layerAlphaLabels_[i]->setLabel(std::to_string(static_cast<int>(layers[i]->getAlpha() * 127)));
        } else {
            layerAlphaLabels_[i]->setLabel("");
        }
    }
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
        Status::instance().reload = true;
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
        Status::instance().forward = true;
    } else if (name == "Previous scene") {
        Status::instance().backward = true;
    } else if (name == "Append scene") {
        show_->appendScene();
    } else if (name == "Load config") {
        auto openFileResult = ofSystemLoadDialog("Select config file");
        if (openFileResult.bSuccess) {
            if (!show_->fromXml(openFileResult.fileName)) {
                ofLog(OF_LOG_WARNING, "Cannot load config file %s, creating default scene instead.", openFileResult.fileName.c_str());
                show_->appendScene();
            }
            Status::instance().reload = true;
        }
    } else if (name == "Save config") {
        auto openFileResult = ofSystemSaveDialog("config.xml", "Save config as");
        if (openFileResult.bSuccess) {
            if (!show_->toXml(openFileResult.fileName)) {
                ofLog(OF_LOG_WARNING, "Cannot save config file to %s.", openFileResult.fileName.c_str());
            }
        }
    } else {
        ofLog(OF_LOG_WARNING, "Unassigned button \"%s\" pressed.", name.c_str());
    }
}

void Gui::onLayerMidiInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    const auto note = static_cast<midiNote>(std::stoi(e.text));
    show_->scenes_[show_->currentIdx_].layers[idx].midiMap = { note };
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setMapping({ note });
}

void Gui::onLayerCCInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    const auto control = static_cast<midiNote>(std::stoi(e.text));
    //show_->scenes_[show_->currentIdx_].layers[idx].midiMap = { note }; TODO
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setAlphaControl(control);
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
    const auto idx = std::stoi(e.target->getName());
    const auto blendMode = static_cast<Layer::BlendMode>(e.child);
    show_->scenes_[show_->currentIdx_].layers[idx].blendMode = blendMode;
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setBlendMode(blendMode);
}

void Gui::onLayerPlayToggle(ofxDatGuiToggleEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    if (currentScene_) {
        currentScene_->playPauseLayer(idx);
        Status::instance().redraw = true;
    }
}

void Gui::onEffectPlayToggle(ofxDatGuiToggleEvent e)
{
    const auto type = static_cast<Effect::Type>(std::stoi(e.target->getName()));
    if (currentScene_)
        currentScene_->playPauseEffect(type);
}

void Gui::onLayerMuteToggle(ofxDatGuiToggleEvent e)
{
    const auto mute = e.checked;
    const auto idx = std::stoi(e.target->getName());
    if (currentScene_)
        currentScene_->layers_[idx]->setMute(mute);
    if (mute)
        layerPlayToggles_[idx]->setChecked(false);
}

void Gui::onEffectMuteToggle(ofxDatGuiToggleEvent e)
{
    const auto mute = e.checked;
    const auto idx = std::stoi(e.target->getName());
    const auto type = static_cast<Effect::Type>(idx);
    if (currentScene_)
        currentScene_->effects_[type].setMute(mute);
    if (mute)
        effectPlayToggles_[idx]->setChecked(false);
}

void Gui::addBlank(ofxDatGui * panel)
{
    auto blank = panel->addButton({});
    blank->setEnabled(false);
    blank->setBackgroundColor(BACKGROUND_COLOR);
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

void Gui::setupControlPanel(glm::ivec2& pos)
{
    controlPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    controlPanel_->setTheme(&headerTheme_);
    pos.x += controlPanel_->getWidth() + 2 * DELTA;

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

    controlPanel_->addBreak();
    controlPanel_->addFRM()->setLabel("fps:");
}

void Gui::setupPlayPanel(glm::ivec2& pos, int w)
{
    playPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    playPanel_->setTheme(&commonTheme_);
    playPanel_->setWidth(w);
    pos.x += playPanel_->getWidth();
    playPanel_->addLabel("Play");
    playPanel_->addBreak();

    for (auto i = 0; i < layerPlayToggles_.size(); ++i) {
        layerPlayToggles_[i] = playPanel_->addToggle({});
        layerPlayToggles_[i]->onToggleEvent(this, &Gui::onLayerPlayToggle);
        layerPlayToggles_[i]->setWidth(w, 0); // This doesn't seem to work right
        layerPlayToggles_[i]->setName(std::to_string(i));
    }

    playPanel_->addBreak();
    addBlank(playPanel_.get());
    playPanel_->addBreak();

    for (auto i = 0; i < effectPlayToggles_.size(); ++i) {
        effectPlayToggles_[i] = playPanel_->addToggle({});
        effectPlayToggles_[i]->onToggleEvent(this, &Gui::onEffectPlayToggle);
        effectPlayToggles_[i]->setWidth(w, 0); // This doesn't seem to work right
        effectPlayToggles_[i]->setName(std::to_string(i));
    }
}

void Gui::setupMutePanel(glm::ivec2& pos, int w)
{
    mutePanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    mutePanel_->setTheme(&commonTheme_);
    mutePanel_->setWidth(w);
    pos.x += mutePanel_->getWidth();
    mutePanel_->addLabel("Mute");
    mutePanel_->addBreak();

    for (auto i = 0; i < layerMuteToggles_.size(); ++i) {
        layerMuteToggles_[i] = mutePanel_->addToggle({});
        layerMuteToggles_[i]->onToggleEvent(this, &Gui::onLayerMuteToggle);
        layerMuteToggles_[i]->setWidth(w, 0); // This doesn't seem to work right
        layerMuteToggles_[i]->setName(std::to_string(i));
    }

    mutePanel_->addBreak();
    addBlank(mutePanel_.get());
    mutePanel_->addBreak();

    for (auto i = 0; i < effectMuteToggles_.size(); ++i) {
        effectMuteToggles_[i] = mutePanel_->addToggle({});
        effectMuteToggles_[i]->onToggleEvent(this, &Gui::onEffectMuteToggle);
        effectMuteToggles_[i]->setWidth(w, 0); // This doesn't seem to work right
        effectMuteToggles_[i]->setName(std::to_string(i));
    }
}

void Gui::setupVideoFxPanel(glm::ivec2& pos)
{
    // Videos & FX panel
    videoFxPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    videoFxPanel_->setTheme(&commonTheme_);
    videoFxPanel_->setWidth(14 * DELTA);
    pos.x += videoFxPanel_->getWidth();
    videoFxPanel_->addLabel("Video")->setTheme(&headerTheme_);
    videoFxPanel_->addBreak();

    for (auto i = 0; i < layerButtons_.size(); ++i) {
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
}

void Gui::setupMidiPanel(glm::ivec2& pos, int w)
{
    midiPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    midiPanel_->setTheme(&commonTheme_);
    midiPanel_->setWidth(w);
    pos.x += midiPanel_->getWidth();
    midiPanel_->addLabel("MIDI");
    midiPanel_->addBreak();
    for (auto i = 0; i < layerMidiInputs_.size(); ++i) {
        layerMidiInputs_[i] = midiPanel_->addTextInput({});
        layerMidiInputs_[i]->setName(std::to_string(i));
        layerMidiInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        layerMidiInputs_[i]->onTextInputEvent(this, &Gui::onLayerMidiInput);
        layerMidiInputs_[i]->setWidth(w, 0); // This doesn't seem to work right
    }

    midiPanel_->addBreak();
    addBlank(midiPanel_.get());
    midiPanel_->addBreak();

    for (auto i = 0; i < effectMidiInputs_.size(); ++i) {
        effectMidiInputs_[i] = midiPanel_->addTextInput({});
        effectMidiInputs_[i]->setName(std::to_string(i));
        effectMidiInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        effectMidiInputs_[i]->onTextInputEvent(this, &Gui::onEffectMidiInput);
        effectMidiInputs_[i]->setWidth(w, 0); // This doesn't seem to work right
    }
}

void Gui::setupMidiCcPanel(glm::ivec2& pos, int w)
{
    midiCcPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    midiCcPanel_->setTheme(&commonTheme_);
    midiCcPanel_->setWidth(w);
    pos.x += midiCcPanel_->getWidth();
    midiCcPanel_->addLabel("CC");
    midiCcPanel_->addBreak();
    for (auto i = 0; i < layerCCInputs_.size(); ++i) {
        layerCCInputs_[i] = midiCcPanel_->addTextInput({});
        layerCCInputs_[i]->setName(std::to_string(i));
        layerCCInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        layerCCInputs_[i]->onTextInputEvent(this, &Gui::onLayerCCInput);
        layerCCInputs_[i]->setWidth(w, 0); // This doesn't seem to work right
    }

    midiCcPanel_->addBreak();
}
void Gui::setuAlphaPanel(glm::ivec2& pos)
{
    alphaPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    alphaPanel_->setTheme(&commonTheme_);
    alphaPanel_->setWidth(2.5 * DELTA);
    pos.x += alphaPanel_->getWidth();
    alphaPanel_->addLabel("Alpha");
    alphaPanel_->addBreak();
    for (auto& layerAlphaLabel : layerAlphaLabels_) {
        layerAlphaLabel = alphaPanel_->addLabel("");
    }

    alphaPanel_->addBreak();
}

void Gui::setupBlendModePanel(glm::ivec2& pos)
{
    blendModePanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    blendModePanel_->setTheme(&commonTheme_);
    blendModePanel_->setWidth(6 * DELTA);
    pos.x += blendModePanel_->getWidth();
    blendModePanel_->addLabel("Blending Mode");
    blendModePanel_->addBreak();

    std::vector<string> options;
    for (auto i = static_cast<int>(Layer::BlendMode::Normal); i < static_cast<int>(Layer::BlendMode::Count); ++i)
        options.push_back(Layer::c_str(static_cast<Layer::BlendMode>(i)));

    for (int i = 0; i < blendModeDropdowns_.size(); ++i) {
        blendModeDropdowns_[i] = blendModePanel_->addDropdown("Select...", options);
        blendModeDropdowns_[i]->setName(std::to_string(i));
        blendModeDropdowns_[i]->select(static_cast<int>(Layer::BlendMode::Normal));
        blendModeDropdowns_[i]->onDropdownEvent(this, &Gui::onBlendModeDropdown);
    }
    blendModePanel_->addBreak();
}
