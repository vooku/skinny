#include "Gui.h"
#include "Status.h"

const std::string Gui::Btn::NEXT = "Next scene";
const std::string Gui::Btn::PREV = "Previous scene";
const std::string Gui::Btn::APPEND = "Append scene";
const std::string Gui::Btn::SAVE = "Save";
const std::string Gui::Btn::SAVE_AS = "Save as";
const std::string Gui::Btn::LOAD = "Load";

const ofColor Gui::BACKGROUND_COLOR = { 45, 45, 48 };

Gui::Gui(ShowDescription& show) :
    currentScene_(nullptr),
    showDescription_(show)
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
    setupRetriggerPanel(pos);
    setupBlendModePanel(pos);
}

void Gui::draw() const
{
    ofBackground(BACKGROUND_COLOR);

    if (std::chrono::system_clock::now() - msg_.start < msg_.duration) {
        fonts_.italic.drawString(msg_.msg, 2 * DELTA, controlPanel_->getHeight() + 2 * DELTA);
    }

    if (controlPanel_) controlPanel_->draw();
    //if (playPanel_) playPanel_->draw();
    //if (mutePanel_) mutePanel_->draw();
    //if (videoFxPanel_) videoFxPanel_->draw();
    //if (midiPanel_) midiPanel_->draw();
    //if (midiCcPanel_) midiCcPanel_->draw();
    //if (blendModePanel_) blendModePanel_->draw();
}

void Gui::reload(std::shared_ptr<Scene> newScene)
{
    currentScene_ = newScene;
    sceneNameInput_->setText(currentScene_->getName());
    masterAlphaInput_->setText(std::to_string(currentScene_->getAlphaControl()));
    midiChannelInput_->setText(std::to_string(showDescription_.getMidiChannel()));

    // layers
    //for (auto& toggle : layerPlayToggles_) {
    //    toggle->setChecked(false);
    //}

    auto& layers = currentScene_->getLayers();
    assert(layerButtons_.size() == layers.size());

    for (auto i = 0; i < MAX_LAYERS; ++i) {
        layerPlayToggles_[i]->setChecked(false);
        layerMuteToggles_[i]->setChecked(false);

        if (layers[i]) {
            layerButtons_[i]->setLabel(layers[i]->getName());
            layerMidiInputs_[i]->setText(std::to_string(layers[i]->getNote()));
            layerCCInputs_[i]->setText(std::to_string(layers[i]->getAlphaControl()));
            layerAlphaLabels_[i]->setLabel(std::to_string(static_cast<int>(layers[i]->getAlpha() * 127)));
            layerRetriggerToggles_[i]->setChecked(layers[i]->getRetrigger());
            blendModeDropdowns_[i]->select(static_cast<int>(layers[i]->getBlendMode()));
        }
        else {
            layerButtons_[i]->setLabel("Click to load a video");
            layerMidiInputs_[i]->setText("");
            layerCCInputs_[i]->setText("");
            layerRetriggerToggles_[i]->setChecked(false);
            blendModeDropdowns_[i]->select(0);
        }
    }

    // effects
    for (auto& toggle : effectPlayToggles_) {
        toggle->setChecked(false);
    }

    // TODO
    //auto& effects = currentScene_->getEffects();
    //for (auto i = 0; i < static_cast<int>(Effect::Type::Count); ++i) {
    //    auto type = static_cast<Effect::Type>(i);
    //    effectButtons_[i]->setLabel(Effect::c_str(type));
    //    effectMidiInputs_[i]->setText(std::to_string(effects.at(type).getNote()));
    //}

    draw();
}

void Gui::update()
{
    if (currentScene_) {
        masterAlphaInput_->setLabel(std::to_string(static_cast<int>(currentScene_->getAlpha() * 127)));

        auto& layers = currentScene_->getLayers();
        for (auto i = 0; i < MAX_LAYERS; ++i) {
            if (layers[i]) {
                layerAlphaLabels_[i]->setLabel(std::to_string(static_cast<int>(layers[i]->getAlpha() * 127)));
            }
            else {
                layerAlphaLabels_[i]->setLabel("");
            }
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

void Gui::displayMessage(const std::string& msg, int duration)
{
    msg_.msg = msg;
    msg_.duration = std::chrono::milliseconds{ duration };
    msg_.start = std::chrono::system_clock::now();
}

void Gui::onLayerButton(ofxDatGuiButtonEvent e)
{
    auto openFileResult = ofSystemLoadDialog("Select a video on layer " + e.target->getName());
    if (openFileResult.bSuccess) {
        auto idx = std::stoi(e.target->getName());
        auto& layerDescription = showDescription_.scenes_[showDescription_.currentIdx_].layers[idx];
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

void Gui::onControlButton(ofxDatGuiButtonEvent e)
{
    auto save = [&](const std::string& path) {
        if (!showDescription_.toXml(path)) {
            ofLog(OF_LOG_WARNING, "Cannot save config file to %s.", path.c_str());
            displayMessage("Cannot save config file to " + path, 1000);
        } else {
            configPath_ = path;
            displayMessage("Saved!", 1000);
        }
    };

    auto saveAs = [&]() {
        auto openFileResult = ofSystemSaveDialog("config.xml", "Save config as");
        if (openFileResult.bSuccess) {
            save(openFileResult.filePath);
        }
    };

    const auto name = e.target->getName();
    if (name == Btn::NEXT) {
        Status::instance().forward = true;
    } else if (name == Btn::PREV) {
        Status::instance().backward = true;
    } else if (name == Btn::APPEND) {
        showDescription_.appendScene();
    } else if (name == Btn::LOAD) {
        auto openFileResult = ofSystemLoadDialog("Select config file");
        if (openFileResult.bSuccess) {
            if (!showDescription_.fromXml(openFileResult.filePath)) {
                ofLog(OF_LOG_WARNING, "Cannot load config file %s, creating default scene instead.", openFileResult.fileName.c_str());
                showDescription_.appendScene();
            } else {
                configPath_ = openFileResult.filePath;
            }
            Status::instance().reload = true;
        }
    } else if (name == Btn::SAVE) {
        if (!configPath_.empty()) {
            save(configPath_);
        } else {
            saveAs();
        }
    } else if (name == Btn::SAVE_AS) {
        saveAs();
    } else {
        ofLog(OF_LOG_WARNING, "Unassigned button \"%s\" pressed.", name.c_str());
    }
}

void Gui::onLayerMidiInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    const auto note = static_cast<midiNote>(std::stoi(e.text));
    showDescription_.scenes_[showDescription_.currentIdx_].layers[idx].note = note ;
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setNote(note);
}

void Gui::onLayerCcInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    const auto control = static_cast<midiNote>(std::stoi(e.text));
    showDescription_.scenes_[showDescription_.currentIdx_].layers[idx].alphaControl = control;
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setAlphaControl(control);
}

void Gui::onMasterAlphaCcInput(ofxDatGuiTextInputEvent e)
{
    const auto control = static_cast<midiNote>(std::stoi(e.text));
    showDescription_.scenes_[showDescription_.currentIdx_].alphaControl = control;
    if (currentScene_)
        currentScene_->setAlphaControl(control);
}

void Gui::onEffectMidiInput(ofxDatGuiTextInputEvent e)
{
    // TODO
    //auto idx = std::stoi(e.target->getName());
    //auto note = static_cast<midiNote>(std::stoi(e.text));
    //showDescription_.scenes_[showDescription_.currentIdx_].effects[idx].note = note;
    //if (currentScene_ && currentScene_->layers_[idx])
    //    currentScene_->effects_[static_cast<Effect::Type>(idx)].setNote(note);
}

void Gui::onSceneNameInput(ofxDatGuiTextInputEvent e)
{
    showDescription_.scenes_[showDescription_.currentIdx_].name = e.text;
    if (currentScene_)
        currentScene_->name_ = e.text;
}

void Gui::onMidiChannelInput(ofxDatGuiTextInputEvent e)
{
    auto channel = std::min(std::max(std::stoi(e.text), 1), 16);
    showDescription_.setMidiChannel(channel);
    midiChannelInput_->setText(std::to_string(channel));
}

void Gui::onBlendModeDropdown(ofxDatGuiDropdownEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    const auto blendMode = static_cast<Layer::BlendMode>(e.child);
    showDescription_.scenes_[showDescription_.currentIdx_].layers[idx].blendMode = blendMode;
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
    // TODO
    //const auto type = static_cast<Effect::Type>(std::stoi(e.target->getName()));
    //if (currentScene_)
    //    currentScene_->playPauseEffect(type);
}

void Gui::onLayerMuteToggle(ofxDatGuiToggleEvent e)
{
    const auto mute = e.checked;
    const auto idx = std::stoi(e.target->getName());
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setMute(mute);
    if (mute)
        layerPlayToggles_[idx]->setChecked(false);
}

void Gui::onEffectMuteToggle(ofxDatGuiToggleEvent e)
{
    // TODO
    //const auto mute = e.checked;
    //const auto idx = std::stoi(e.target->getName());
    //const auto type = static_cast<Effect::Type>(idx);
    //if (currentScene_)
    //    currentScene_->effects_[type].setMute(mute);
    //if (mute)
    //    effectPlayToggles_[idx]->setChecked(false);
}

void Gui::onLayerRetriggerToggle(ofxDatGuiToggleEvent e)
{
    const auto retrigger = e.checked;
    const auto idx = std::stoi(e.target->getName());
    showDescription_.scenes_[showDescription_.currentIdx_].layers[idx].retrigger = retrigger;
    if (currentScene_ && currentScene_->layers_[idx])
        currentScene_->layers_[idx]->setRetrigger(retrigger);
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

    controlButtons_.push_back(controlPanel_->addButton(Btn::NEXT));
    controlButtons_.push_back(controlPanel_->addButton(Btn::PREV));
    controlButtons_.push_back(controlPanel_->addButton(Btn::APPEND));
    controlPanel_->addBreak();
    controlButtons_.push_back(controlPanel_->addButton(Btn::SAVE));
    controlButtons_.push_back(controlPanel_->addButton(Btn::SAVE_AS));
    controlButtons_.push_back(controlPanel_->addButton(Btn::LOAD));
    for (auto& btn : controlButtons_)
        btn->onButtonEvent(this, &Gui::onControlButton);

    controlPanel_->addBreak();
    //addBlank(controlPanel_.get());
    controlPanel_->addLabel("Master alpha");
    masterAlphaInput_ = controlPanel_->addTextInput("");
    masterAlphaInput_->setInputType(ofxDatGuiInputType::NUMERIC);
    masterAlphaInput_->onTextInputEvent(this, &Gui::onMasterAlphaCcInput);
    //addBlank(controlPanel_.get());

    controlPanel_->addBreak();
    midiChannelInput_ = controlPanel_->addTextInput("Channel");
    midiChannelInput_->setInputType(ofxDatGuiInputType::NUMERIC);
    midiChannelInput_->setText(std::to_string(showDescription_.getMidiChannel()));
    midiChannelInput_->onTextInputEvent(this, &Gui::onMidiChannelInput);

    controlPanel_->addBreak();
    controlPanel_->addFRM()->setLabel("fps");
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
        layerCCInputs_[i] = midiCcPanel_->addTextInput("");
        layerCCInputs_[i]->setName(std::to_string(i));
        layerCCInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        layerCCInputs_[i]->onTextInputEvent(this, &Gui::onLayerCcInput);
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

void Gui::setupRetriggerPanel(glm::ivec2 & pos)
{
    retriggerPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    retriggerPanel_->setTheme(&commonTheme_);
    retriggerPanel_->setWidth(2.5 * DELTA);
    pos.x += retriggerPanel_->getWidth();
    retriggerPanel_->addLabel("Re");
    retriggerPanel_->addBreak();
    for (auto i = 0; i < layerRetriggerToggles_.size(); ++i) {
        layerRetriggerToggles_[i] = retriggerPanel_->addToggle({});
        layerRetriggerToggles_[i]->onToggleEvent(this, &Gui::onLayerRetriggerToggle);
        layerRetriggerToggles_[i]->setName(std::to_string(i));
    }

    retriggerPanel_->addBreak();
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
