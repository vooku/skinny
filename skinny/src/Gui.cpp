#include "Gui.h"
#include "Status.h"

#include <filesystem>

namespace skinny {

const std::string Gui::Btn::NEXT = "Next scene";
const std::string Gui::Btn::PREV = "Previous scene";
const std::string Gui::Btn::JUMP = "Jump to scene";
const std::string Gui::Btn::NEW = "New scene";
const std::string Gui::Btn::SAVE = "Save";
const std::string Gui::Btn::SAVE_AS = "Save as";
const std::string Gui::Btn::LOAD = "Load";

const ofColor Gui::BACKGROUND_COLOR = { 45, 45, 48 };

Gui::Gui(ShowDescription& showDescription) :
    showDescription_(showDescription)
{
}

void Gui::setup()
{
    fonts_.regular.load(FONT_REGULAR, fonts_.sizeRegular, true, false);
    fonts_.italic.load(FONT_ITALIC, fonts_.sizeItalic, true, false);

    glm::ivec2 pos{ DELTA, DELTA };
    const auto toggleWidth = 2 * DELTA;
    const auto midiInWidth = 3 * DELTA;

    setupControlPanel(pos);
    setupPlayPanel(pos, toggleWidth);
    setupMutePanel(pos, toggleWidth);
    setupVideoFxPanel(pos);
    setupMidiPanel(pos, midiInWidth);
    setupCcPanel(pos, midiInWidth);
    setupAlphaPanel(pos);
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

std::string buildJumpToLabel(int idx, int size)
{
    return std::to_string(idx) + "/" + std::to_string(size);
}

void Gui::reload()
{
    if (!show_)
        return;

    sceneNameInput_->setText(show_->getCurrentScene()->getName());
    sceneNameInput_->setLabel("Scene");
    jumpToInput_->setLabel(buildJumpToLabel(showDescription_.currentIdx_ + 1, showDescription_.getSize()));
    jumpToInput_->setText(std::to_string(showDescription_.currentIdx_ + 1));
    masterAlphaInput_->setText(std::to_string(show_->getAlphaControl()));
    midiChannelInput_->setText(std::to_string(showDescription_.getMidiChannel()));

    // layers
    const auto& layers = show_->getCurrentScene()->getLayers();
    assert(layerButtons_.size() == layers.size());

    for (auto i = 0; i < MAX_LAYERS; ++i) {
        layerPlayToggles_[i]->setChecked(false);
        layerMuteToggles_[i]->setChecked(false);

        if (layers[i]) {
            const auto name = layers[i]->getName();
            const auto label =
                name.length() > MAX_LABEL_LENGTH ?
                name.substr(0, MAX_LABEL_LENGTH - 3) + "..." :
                name;
            layerButtons_[i]->setLabel(label);
            layerMidiInputs_[i]->setText(std::to_string(layers[i]->getNote()));
            layerCCInputs_[i]->setText(std::to_string(layers[i]->getCc()));
            layerAlphaLabels_[i]->setLabel(std::to_string(static_cast<int>(layers[i]->getAlpha() * 127)));
            layerRetriggerToggles_[i]->setChecked(layers[i]->getRetrigger());
            blendModeDropdowns_[i]->select(static_cast<int>(layers[i]->getBlendMode()));
        }
        else {
            layerButtons_[i]->setLabel("Click to load a video");
            layerMidiInputs_[i]->setText("");
            layerCCInputs_[i]->setText("");
            layerRetriggerToggles_[i]->setChecked(false);
            blendModeDropdowns_[i]->select(static_cast<int>(DEFAULT_BLEND_MODE));
        }
    }

    // effects
    const auto& effects = show_->effects_;
    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        effectPlayToggles_[i]->setChecked(false);
        effectMuteToggles_[i]->setChecked(false);
        effectDropdowns_[i]->select(static_cast<int>(effects[i]->type));
        effectMidiInputs_[i]->setText(std::to_string(effects[i]->getNote()));
        effectCCInputs_[i]->setText(std::to_string(effects[i]->getCc()));
        effectParamLabels_[i]->setLabel(std::to_string(effects[i]->param_));
    }

    draw();
}

void Gui::update()
{
    if (!show_)
        return;

    if (!configName_.empty()) {
      ofSetWindowTitle(configName_);
    }
    
    masterAlphaInput_->setLabel(std::to_string(static_cast<int>(show_->getAlpha() * 127)));

    const auto& layers = show_->getCurrentScene()->getLayers();
    for (auto i = 0; i < MAX_LAYERS; ++i) {
        if (layers[i]) {
            layerAlphaLabels_[i]->setLabel(std::to_string(static_cast<int>(layers[i]->getAlpha() * 127)));
        }
        else {
            layerAlphaLabels_[i]->setLabel("");
        }
    }

    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        if (show_->effects_[i]) {
            effectParamLabels_[i]->setLabel(std::to_string(show_->effects_[i]->param_));
            // #TODO This is the wrong place for this!!!
            showDescription_.effects_[i].param = show_->effects_[i]->param_;
        }
    }
}

void Gui::setShow(std::shared_ptr<Show> show)
{
    show_ = show;
}

void Gui::setActiveLayer(int idx, bool active)
{
    if (idx < layerPlayToggles_.size()) {
        layerPlayToggles_[idx]->setChecked(active);
    }
    else {
        ofLog(OF_LOG_WARNING, "Trying to activate layer #%d out of bounds", idx);
    }
}

void Gui::setActiveEffect(int idx, bool active)
{
    if (idx < effectPlayToggles_.size()) {
        effectPlayToggles_[idx]->setChecked(active);
    }
    else {
        ofLog(OF_LOG_WARNING, "Trying to activate effect #%d out of bounds", idx);
    }
}

int Gui::getJumpToIndex() const
{
    return std::stoi(jumpToInput_->getText()) - 1;
}

void Gui::resetJumpToIndex()
{
    jumpToInput_->setText(std::to_string(showDescription_.currentIdx_ + 1));
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
        Status::instance().loadDir = LoadDir::Current;
    }
}

void Gui::onControlButton(ofxDatGuiButtonEvent e)
{
    auto save = [&](const std::string& path) {
        ofxXmlSettings config;
        showDescription_.toXml(config);
        if (!config.saveFile(path)) {
            ofLog(OF_LOG_WARNING, "Cannot save config file to %s.", path.c_str());
            displayMessage("Cannot save config file to " + path, 1000);
        } else {
            configPath_ = path;
            configName_ = std::filesystem::path(configPath_).filename().string();
            displayMessage("Saved!", 1000);
        }
    };

    auto saveAs = [&]() {
        auto openFileResult = ofSystemSaveDialog("config.xml", "Save config as");
        if (openFileResult.bSuccess) {
            save(openFileResult.filePath);
        }
    };

    auto load = [&]() {
        auto openFileResult = ofSystemLoadDialog("Select config file");
        if (openFileResult.bSuccess) {
            ofxXmlSettings config;
            if (config.loadFile(openFileResult.filePath) && showDescription_.fromXml(config)) {
                configPath_ = openFileResult.filePath;
                configName_ = std::filesystem::path(configPath_).filename().string();
            }
            else {
                ofLog(OF_LOG_WARNING, "Cannot load config file %s, creating default scene instead.", openFileResult.fileName.c_str());
                showDescription_.appendScene();
            }
            Status::instance().loadDir = LoadDir::Current;
        }
    };

    const auto name = e.target->getName();
    if (name == Btn::NEXT) {
        Status::instance().loadDir = LoadDir::Forward;
    } else if (name == Btn::PREV) {
        Status::instance().loadDir = LoadDir::Backward;
    } else if (name == Btn::JUMP) {
        Status::instance().loadDir = LoadDir::Jump;
        Status::instance().jumpToIndex = getJumpToIndex();
    }
    else if (name == Btn::NEW) {
        showDescription_.appendScene();
        jumpToInput_->setLabel(buildJumpToLabel(showDescription_.currentIdx_ + 1, showDescription_.getSize()));
        Status::instance().loadDir = LoadDir::Jump;
        Status::instance().jumpToIndex = showDescription_.getSize() - 1;
    } else if (name == Btn::LOAD) {
        load();
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
    if (show_ && show_->getCurrentScene()->layers_[idx])
        show_->getCurrentScene()->layers_[idx]->setNote(note);
}

void Gui::onLayerCcInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    const auto control = static_cast<midiNote>(std::stoi(e.text));
    showDescription_.scenes_[showDescription_.currentIdx_].layers[idx].cc = control;
    if (show_ && show_->getCurrentScene()->layers_[idx])
        show_->getCurrentScene()->layers_[idx]->setCc(control);
}

void Gui::onMasterAlphaCcInput(ofxDatGuiTextInputEvent e)
{
    const auto control = static_cast<midiNote>(std::stoi(e.text));
    showDescription_.alphaControl_ = control;
    if (show_)
        show_->setAlphaControl(control);
}

void Gui::onEffectMidiInput(ofxDatGuiTextInputEvent e)
{
    auto idx = std::stoi(e.target->getName());
    auto note = static_cast<midiNote>(std::stoi(e.text));
    showDescription_.effects_[idx].note = note;
    show_->effects_[idx]->setNote(note);
}


void Gui::onEffectCcInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    const auto cc = static_cast<midiNote>(std::stoi(e.text));
    showDescription_.effects_[idx].cc = cc;
    if (show_)
        show_->effects_[idx]->setCc(cc);
}

void Gui::onSceneNameInput(ofxDatGuiTextInputEvent e)
{
    showDescription_.scenes_[showDescription_.currentIdx_].name = e.text;
    if (show_->getCurrentScene())
        show_->getCurrentScene()->name_ = e.text;
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
    const auto blendMode = static_cast<BlendMode>(e.child);
    showDescription_.scenes_[showDescription_.currentIdx_].layers[idx].blendMode = blendMode;
    if (show_ && show_->getCurrentScene()->layers_[idx])
        show_->getCurrentScene()->layers_[idx]->setBlendMode(blendMode);
}

void Gui::onEffectDropdown(ofxDatGuiDropdownEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    const auto type = static_cast<EffectType>(e.child);
    const auto note = show_->effects_[idx]->getNote();
    const auto cc = show_->effects_[idx]->getCc();
    const auto param = show_->effects_[idx]->param_;
    showDescription_.effects_[idx].type = type;
    if (show_)
        show_->effects_[idx].reset(new Effect(idx, type, note, cc, param));
}

void Gui::onLayerPlayToggle(ofxDatGuiToggleEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    if (show_->getCurrentScene()) {
        show_->getCurrentScene()->playPauseLayer(idx);
    }
}

void Gui::onEffectPlayToggle(ofxDatGuiToggleEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    show_->playPauseEffect(idx);
}

void Gui::onLayerMuteToggle(ofxDatGuiToggleEvent e)
{
    const auto mute = e.checked;
    const auto idx = std::stoi(e.target->getName());
    if (show_ && show_->getCurrentScene()->layers_[idx])
        show_->getCurrentScene()->layers_[idx]->setMute(mute);
    if (mute)
        layerPlayToggles_[idx]->setChecked(false);
}

void Gui::onEffectMuteToggle(ofxDatGuiToggleEvent e)
{
    const auto mute = e.checked;
    const auto idx = std::stoi(e.target->getName());
    show_->effects_[idx]->setMute(mute);
    // TODO if (mute)
    //    effectPlayToggles_[idx]->setChecked(false);
}

void Gui::onLayerRetriggerToggle(ofxDatGuiToggleEvent e)
{
    const auto retrigger = e.checked;
    const auto idx = std::stoi(e.target->getName());
    showDescription_.scenes_[showDescription_.currentIdx_].layers[idx].retrigger = retrigger;
    if (show_ && show_->getCurrentScene()->layers_[idx])
        show_->getCurrentScene()->layers_[idx]->setRetrigger(retrigger);
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
    font.file = FONT_REGULAR;

    init();
}

Gui::HeaderTheme::HeaderTheme() :
    CommonTheme()
{
    layout.height *= 1.4;
    font.size = Gui::Fonts::sizeItalic;
    font.file = FONT_ITALIC;

    init();
}

void Gui::setupControlPanel(glm::ivec2& pos)
{
    controlPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    controlPanel_->setTheme(&headerTheme_);
    pos.x += controlPanel_->getWidth() + 2 * DELTA;

    sceneNameInput_ = controlPanel_->addTextInput("Scene " + std::to_string(showDescription_.getSceneIndex() + 1));
    sceneNameInput_->setText(show_ ? show_->getCurrentScene()->getName() : "Enter scene name");
    sceneNameInput_->onTextInputEvent(this, &Gui::onSceneNameInput);

    controlButtons_.push_back(controlPanel_->addButton(Btn::NEXT));
    controlButtons_.push_back(controlPanel_->addButton(Btn::PREV));
    jumpToInput_ = controlPanel_->addTextInput("");
    jumpToInput_->setInputType(ofxDatGuiInputType::NUMERIC);
    jumpToInput_->setLabel(buildJumpToLabel(0, 0));
    jumpToInput_->setText("0");
    controlButtons_.push_back(controlPanel_->addButton(Btn::JUMP));
    controlButtons_.push_back(controlPanel_->addButton(Btn::NEW));
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

    std::vector<string> options;
    for (auto i = 0u; i < MAX_EFFECTS; ++i)
        options.push_back(c_str(static_cast<EffectType>(i)));

    for (auto i = 0; i < effectDropdowns_.size(); ++i) {
        effectDropdowns_[i] = videoFxPanel_->addDropdown("Select...", options);
        effectDropdowns_[i]->setName(std::to_string(i));
        effectDropdowns_[i]->select(i);
        effectDropdowns_[i]->onDropdownEvent(this, &Gui::onEffectDropdown);
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

void Gui::setupCcPanel(glm::ivec2& pos, int w)
{
    ccPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    ccPanel_->setTheme(&commonTheme_);
    ccPanel_->setWidth(w);
    pos.x += ccPanel_->getWidth();
    ccPanel_->addLabel("CC");
    ccPanel_->addBreak();
    for (auto i = 0; i < layerCCInputs_.size(); ++i) {
        layerCCInputs_[i] = ccPanel_->addTextInput("");
        layerCCInputs_[i]->setName(std::to_string(i));
        layerCCInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        layerCCInputs_[i]->onTextInputEvent(this, &Gui::onLayerCcInput);
        layerCCInputs_[i]->setWidth(w, 0); // This doesn't seem to work right
    }

    ccPanel_->addBreak();
    addBlank(ccPanel_.get());
    ccPanel_->addBreak();

    for (auto i = 0; i < effectCCInputs_.size(); ++i) {
        effectCCInputs_[i] = ccPanel_->addTextInput({});
        effectCCInputs_[i]->setName(std::to_string(i));
        effectCCInputs_[i]->setInputType(ofxDatGuiInputType::NUMERIC);
        effectCCInputs_[i]->onTextInputEvent(this, &Gui::onEffectCcInput);
        effectCCInputs_[i]->setWidth(w, 0); // This doesn't seem to work right
    }
}
void Gui::setupAlphaPanel(glm::ivec2& pos)
{
    alphaPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    alphaPanel_->setTheme(&commonTheme_);
    alphaPanel_->setWidth(2.5 * DELTA);
    pos.x += alphaPanel_->getWidth();
    alphaPanel_->addLabel("Alpha");
    alphaPanel_->addBreak();
    for (auto& label : layerAlphaLabels_) {
        label = alphaPanel_->addLabel("");
    }

    alphaPanel_->addBreak();
    alphaPanel_->addLabel("Para");
    alphaPanel_->addBreak();

    for (auto& label : effectParamLabels_) {
        label = alphaPanel_->addLabel("");
    }
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
    for (auto i = static_cast<int>(BlendMode::Overlay); i < static_cast<int>(BlendMode::Count); ++i)
        options.push_back(c_str(static_cast<BlendMode>(i)));

    for (int i = 0; i < blendModeDropdowns_.size(); ++i) {
        blendModeDropdowns_[i] = blendModePanel_->addDropdown("Select...", options);
        blendModeDropdowns_[i]->setName(std::to_string(i));
        blendModeDropdowns_[i]->onDropdownEvent(this, &Gui::onBlendModeDropdown);
    }
    blendModePanel_->addBreak();
}

} // namespace skinny
