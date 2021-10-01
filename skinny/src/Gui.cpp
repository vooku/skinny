#include "Gui.h"
#include "Status.h"

namespace skinny {

const std::string Gui::Btn::NEXT = "Next scene";
const std::string Gui::Btn::PREV = "Previous scene";
const std::string Gui::Btn::JUMP = "Jump to scene";
const std::string Gui::Btn::NEW = "New scene";
const std::string Gui::Btn::SAVE = "Save";
const std::string Gui::Btn::SAVE_AS = "Save as";
const std::string Gui::Btn::LOAD = "Load";

const ofColor Gui::BACKGROUND_COLOR = { 45, 45, 48 };

//--------------------------------------------------------------
void Gui::setup()
{
    getStatus().gui = shared_from_this();

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
    setupAlphaPanel(pos, midiInWidth);

    auto midiPos = pos;
    
    setupRetriggerPanel(pos);
    setupBlendModePanel(pos);
    setupMidiDevicePanel(midiPos);

    midiDevicesTimer_.setPeriodicEvent(MIDI_DEVICES_REFRESH_PERIOD);
    startThread();

    midiMonitor_.init();
}

//--------------------------------------------------------------
void Gui::draw()
{
    ofBackground(BACKGROUND_COLOR);
    
    //auto y = 0;
    //const auto w = ofGetViewportWidth();
    //const auto h = ofGetViewportHeight();
    //while (y < h) {
    //  ofDrawLine({ 0, y }, { w, y });
    //  y += 26;
    //}

    if (std::chrono::system_clock::now() - msg_.start < msg_.duration) {
        fonts_.italic.drawString(msg_.msg, 2 * DELTA, controlPanel_->getHeight() + 2 * DELTA);
    }

    if (midiDevicePanel_) {
      midiDevicePanel_->setTheme(&commonTheme_, true);
			midiDevicePanel_->update();
      midiDevicePanel_->draw();
    }
    // other panels are drawn automatically
}

//--------------------------------------------------------------
std::string buildJumpToLabel(int idx, int size)
{
    return std::to_string(idx) + "/" + std::to_string(size);
}

//--------------------------------------------------------------
void Gui::reload()
{
    const auto& show = Status::instance().show;
    if (!show)
        return;

    const auto& showDesc = *Status::instance().showDescription;

    sceneNameInput_->setText(show->getCurrentScene()->getName());
    sceneNameInput_->setLabel("Scene");
    jumpToInput_->setLabel(buildJumpToLabel(showDesc.currentIdx_ + 1, showDesc.getSize()));
    jumpToInput_->setText(std::to_string(showDesc.currentIdx_ + 1));
    masterAlphaInput_->setText(std::to_string(show->getAlphaControl()));
    midiChannelInput_->setText(std::to_string(showDesc.getMidiChannel()));

    // layers
    const auto& layers = show->getCurrentScene()->getLayers();
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
            layerAlphaInputs_[i]->setText(std::to_string(static_cast<int>(layers[i]->getAlpha() * MAX_7BIT)));
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
    const auto& effects = show->effects_;
    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        effectPlayToggles_[i]->setChecked(false);
        effectMuteToggles_[i]->setChecked(false);
        effectDropdowns_[i]->select(static_cast<int>(effects[i]->type));
        effectMidiInputs_[i]->setText(std::to_string(effects[i]->getNote()));
        effectCCInputs_[i]->setText(std::to_string(effects[i]->getCc()));
        effectParamInputs_[i]->setText(std::to_string(effects[i]->ccValue_));
    }

    draw();
}

//--------------------------------------------------------------
void Gui::update()
{
    if (getStatus().exit) {
        ofExit();
    }

    if (shouldUpdateDevices_)
    {
      setupMidiDevicePanel();
      shouldUpdateDevices_ = false;
		}

    if (midiMonitorLabel_ != nullptr) {
      midiMonitorLabel_->setLabel(midiMonitor_.getCurrentMsg());
    }

    auto& showDescription = *Status::instance().showDescription;
    const auto& show = Status::instance().show;
    if (!show)
        return;

    if (!configName_.empty()) {
      ofSetWindowTitle(configName_);
    }
    
    masterAlphaInput_->setLabel(std::to_string(static_cast<int>(show->getAlpha() * MAX_7BIT)));

    const auto& layers = show->getCurrentScene()->getLayers();
    for (auto i = 0; i < MAX_LAYERS; ++i) {
        if (layers[i]) {
            layerPlayToggles_[i]->setChecked(layers[i]->isPlaying());
            if (!layerAlphaInputs_[i]->getFocused())
              layerAlphaInputs_[i]->setText(std::to_string(static_cast<int>(layers[i]->getAlpha() * MAX_7BIT)));
        }
        else {
            layerPlayToggles_[i]->setChecked(false);
            layerAlphaInputs_[i]->setText("");
        }
    }

    const auto& effects = show->effects_;
    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        if (effects[i]) {
            effectPlayToggles_[i]->setChecked(effects[i]->isPlaying());
            if (!effectParamInputs_[i]->getFocused())
              effectParamInputs_[i]->setText(std::to_string(effects[i]->ccValue_));
        }
        else {
            effectPlayToggles_[i]->setChecked(false);
        }
    }

    // load video
    if (videoSelector_ != nullptr && !videoSelector_->isThreadRunning()) {
        const auto& ctx = videoSelector_->getContext();

        if (!ctx.path.empty()) {
            auto& layerDescription = showDescription.scenes_[showDescription.currentIdx_].layers[ctx.index];
            if (layerDescription.valid) {
              layerDescription.path = ctx.path;
            }
            else {
              layerDescription = { ctx.index, ctx.path };
            }

            Status::instance().loadDir = LoadDir::Current;
        }

        videoSelector_.release();
    }

    // save & load config
    if (fileSelector_ != nullptr && !fileSelector_->isThreadRunning()) {
        const auto& path = fileSelector_->getPath();

        if (!path.empty()) {
            if (fileSelector_->isLoading()) {
                ofxXmlSettings config;

                if (config.loadFile(path.string()) && showDescription.fromXml(config)) {
                    configPath_ = path;
                    configName_ = configPath_.filename().string();
                }
                else {
                    ofLog(OF_LOG_WARNING, "Cannot load config file %s, creating default scene instead.", path.c_str());
                    showDescription = {};
                }

                Status::instance().loadDir = LoadDir::Current;
            }
            else {
                save(path);
            }
        }

        fileSelector_.release();
    }
}

//--------------------------------------------------------------
void Gui::exit()
{
	midiMonitor_.done();
  waitForThread();
  getStatus().exit = true;
}

//--------------------------------------------------------------
void Gui::setActiveLayer(int idx, bool active)
{
    if (idx < layerPlayToggles_.size()) {
        layerPlayToggles_[idx]->setChecked(active);
    }
    else {
        ofLog(OF_LOG_WARNING, "Trying to activate layer #%d out of bounds", idx);
    }
}

//--------------------------------------------------------------
void Gui::setActiveEffect(int idx, bool active)
{
    if (idx < effectPlayToggles_.size()) {
        effectPlayToggles_[idx]->setChecked(active);
    }
    else {
        ofLog(OF_LOG_WARNING, "Trying to activate effect #%d out of bounds", idx);
    }
}

//--------------------------------------------------------------
int Gui::getJumpToIndex() const
{
    return std::stoi(jumpToInput_->getText()) - 1;
}

//--------------------------------------------------------------
void Gui::resetJumpToIndex()
{
    auto& showDescription = *Status::instance().showDescription;
    jumpToInput_->setText(std::to_string(showDescription.currentIdx_ + 1));
}

//--------------------------------------------------------------
void Gui::displayMessage(const std::string& msg, int duration)
{
    msg_.msg = msg;
    msg_.duration = std::chrono::milliseconds{ duration };
    msg_.start = std::chrono::system_clock::now();
}

//--------------------------------------------------------------
void Gui::onLayerButton(ofxDatGuiButtonEvent e)
{
    if (videoSelector_ != nullptr) {
        return;
    }        

    const auto index = std::stoi(e.target->getName());
    videoSelector_ = std::make_unique<VideoSelector>(index);
    videoSelector_->startThread();
}

//--------------------------------------------------------------
void Gui::save(std::filesystem::path path)
{
    if (path.extension().string() != DEFAULT_EXTENSION) {
        path += DEFAULT_EXTENSION;
    }

		ofxXmlSettings config;
		auto& showDescription = *Status::instance().showDescription;
    showDescription.toXml(config);
    if (!config.saveFile(path.string())) {
        ofLog(OF_LOG_WARNING, "Cannot save config file to %s.", path.c_str());
        displayMessage("Cannot save config file to " + path.string(), 1000);
    }
    else {
        configPath_ = path;
        configName_ = configPath_.filename().string();
        displayMessage("Saved!", 1000);
    }
}

//--------------------------------------------------------------
void Gui::onControlButton(ofxDatGuiButtonEvent e)
{
    auto saveAs = [&]() {
        if (fileSelector_ != nullptr) {
            return;
        }

        fileSelector_ = std::make_unique<FileSelector>("Save config as", false);
        fileSelector_->startThread();
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
		    auto& showDescription = *Status::instance().showDescription;
        showDescription.appendScene();
        jumpToInput_->setLabel(buildJumpToLabel(showDescription.currentIdx_ + 1, showDescription.getSize()));
        Status::instance().loadDir = LoadDir::Jump;
        Status::instance().jumpToIndex = showDescription.getSize() - 1;
    } else if (name == Btn::LOAD) {
        if (fileSelector_ != nullptr) {
            return;
        }

        fileSelector_ = std::make_unique<FileSelector>("Select config file");
        fileSelector_->startThread();
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

//--------------------------------------------------------------
void Gui::onLayerMidiInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
		const auto note = static_cast<midiNote>(std::stoi(e.text));
		auto& showDescription = *Status::instance().showDescription;
    showDescription.scenes_[showDescription.currentIdx_].layers[idx].note = note;
    auto& show = Status::instance().show;
    if (show && show->getCurrentScene()->layers_[idx])
        show->getCurrentScene()->layers_[idx]->setNote(note);
}

//--------------------------------------------------------------
void Gui::onLayerCcInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
		const auto control = static_cast<midiNote>(std::stoi(e.text));
		auto& showDescription = *Status::instance().showDescription;
    showDescription.scenes_[showDescription.currentIdx_].layers[idx].cc = control;
    auto& show = Status::instance().show;
    if (show && show->getCurrentScene()->layers_[idx])
        show->getCurrentScene()->layers_[idx]->setCc(control);
}

//--------------------------------------------------------------
void Gui::onLayerAlphaInput(ofxDatGuiTextInputEvent e)
{
  const auto idx = std::stoi(e.target->getName());
  const auto alpha = std::stoi(e.text);
	auto& showDescription = *Status::instance().showDescription;
	showDescription.scenes_[showDescription.currentIdx_].layers[idx].alpha = alpha;
	auto& show = Status::instance().show;
	if (show && show->getCurrentScene()->layers_[idx])
		show->getCurrentScene()->layers_[idx]->setAlpha(alpha);
}

//--------------------------------------------------------------
void Gui::onMasterAlphaCcInput(ofxDatGuiTextInputEvent e)
{
    const auto control = static_cast<midiNote>(std::stoi(e.text));
    auto& showDescription = *Status::instance().showDescription;
    showDescription.alphaControl_ = control;
    auto& show = Status::instance().show;
    if (show)
        show->setAlphaControl(control);
}

//--------------------------------------------------------------
void Gui::onEffectMidiInput(ofxDatGuiTextInputEvent e)
{
    auto idx = std::stoi(e.target->getName());
		auto note = static_cast<midiNote>(std::stoi(e.text));
		auto& showDescription = *Status::instance().showDescription;
    showDescription.effects_[idx].note = note;
    auto& show = Status::instance().show;
    show->effects_[idx]->setNote(note);
}

//--------------------------------------------------------------
void Gui::onEffectCcInput(ofxDatGuiTextInputEvent e)
{
    const auto idx = std::stoi(e.target->getName());
		const auto cc = static_cast<midiNote>(std::stoi(e.text));
		auto& showDescription = *Status::instance().showDescription;
    showDescription.effects_[idx].cc = cc;
    auto& show = Status::instance().show;
    if (show)
        show->effects_[idx]->setCc(cc);
}

//--------------------------------------------------------------
void Gui::onEffectParamInput(ofxDatGuiTextInputEvent e)
{
	const auto idx = std::stoi(e.target->getName());
	const auto param = std::stoi(e.text);
	auto& showDescription = *Status::instance().showDescription;
	showDescription.effects_[idx].param = param;
	auto& show = Status::instance().show;
	if (show)
		show->effects_[idx]->setParam(param);
}

//--------------------------------------------------------------
void Gui::onSceneNameInput(ofxDatGuiTextInputEvent e)
{
    auto& showDescription = *Status::instance().showDescription;
    showDescription.scenes_[showDescription.currentIdx_].name = e.text;
    auto& show = Status::instance().show;
    if (show->getCurrentScene())
        show->getCurrentScene()->name_ = e.text;
}

//--------------------------------------------------------------
void Gui::onMidiChannelInput(ofxDatGuiTextInputEvent e)
{
    auto channel = std::min(std::max(std::stoi(e.text), 1), 16);
    auto& showDescription = *Status::instance().showDescription;
    showDescription.setMidiChannel(channel);
    midiChannelInput_->setText(std::to_string(channel));
}

//--------------------------------------------------------------
void Gui::onBlendModeDropdown(ofxDatGuiDropdownEvent e)
{
    const auto idx = std::stoi(e.target->getName());
		const auto blendMode = static_cast<BlendMode>(e.child);
		auto& showDescription = *Status::instance().showDescription;
    showDescription.scenes_[showDescription.currentIdx_].layers[idx].blendMode = blendMode;
    auto& show = Status::instance().show;
    if (show && show->getCurrentScene()->layers_[idx])
        show->getCurrentScene()->layers_[idx]->setBlendMode(blendMode);
}

//--------------------------------------------------------------
void Gui::onEffectDropdown(ofxDatGuiDropdownEvent e)
{
    auto& show = Status::instance().show;
    if (!show)
        return;
    const auto idx = std::stoi(e.target->getName());
    const auto type = static_cast<EffectType>(e.child);
		auto& showDescription = *Status::instance().showDescription;
    showDescription.effects_[idx].type = type;
    getStatus().loadDir = LoadDir::Current;
}

//--------------------------------------------------------------
void Gui::onLayerPlayToggle(ofxDatGuiToggleEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    auto& show = Status::instance().show;
    if (show && show->getCurrentScene()) {
        show->getCurrentScene()->playPauseLayer(idx);
    }
}

//--------------------------------------------------------------
void Gui::onEffectPlayToggle(ofxDatGuiToggleEvent e)
{
    const auto idx = std::stoi(e.target->getName());
    auto& show = Status::instance().show;
    if (show)
        show->playPauseEffect(idx);
}

//--------------------------------------------------------------
void Gui::onLayerMuteToggle(ofxDatGuiToggleEvent e)
{
    const auto mute = e.checked;
    const auto idx = std::stoi(e.target->getName());
    auto& show = Status::instance().show;
    if (show && show->getCurrentScene()->layers_[idx])
        show->getCurrentScene()->layers_[idx]->setMute(mute);
    if (mute)
        layerPlayToggles_[idx]->setChecked(false);
}

//--------------------------------------------------------------
void Gui::onEffectMuteToggle(ofxDatGuiToggleEvent e)
{
    const auto mute = e.checked;
    const auto idx = std::stoi(e.target->getName());
    auto& show = Status::instance().show;
    show->effects_[idx]->setMute(mute);
    // TODO if (mute)
    //    effectPlayToggles_[idx]->setChecked(false);
}

//--------------------------------------------------------------
void Gui::onLayerRetriggerToggle(ofxDatGuiToggleEvent e)
{
    const auto retrigger = e.checked;
		const auto idx = std::stoi(e.target->getName());
		auto& showDescription = *Status::instance().showDescription;
    showDescription.scenes_[showDescription.currentIdx_].layers[idx].retrigger = retrigger;
    auto& show = Status::instance().show;
    if (show && show->getCurrentScene()->layers_[idx])
        show->getCurrentScene()->layers_[idx]->setRetrigger(retrigger);
}

//--------------------------------------------------------------
void Gui::onMidiDeviceToggle(ofxDatGuiToggleEvent e)
{
  const auto deviceName = e.target->getName();
  if (e.checked) {
    const auto portOpen = getStatus().midi->connect(deviceName);
    if (!portOpen) {
      e.target->setChecked(false);
      displayMessage(std::string("Failed to open ") + deviceName + std::string("."));
    }
  }
  else {
    getStatus().midi->disconnect(deviceName);
  }
}

//--------------------------------------------------------------
void Gui::onMidiMonitorToggle(ofxDatGuiToggleEvent e)
{
  midiMonitor_.on_ = e.checked;
  setupMidiMonitorLabel();
}

//--------------------------------------------------------------
void Gui::addBlank(ofxDatGui * panel)
{
    auto blank = panel->addLabel("");
    blank->setBackgroundColor(BACKGROUND_COLOR);
    //blank->setStripeColor(bgColor);
}

//--------------------------------------------------------------
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

//--------------------------------------------------------------
Gui::HeaderTheme::HeaderTheme() :
    CommonTheme()
{
    layout.height *= 1.4;
    font.size = Gui::Fonts::sizeItalic;
    font.file = FONT_ITALIC;

    init();
}

//--------------------------------------------------------------
void Gui::setupControlPanel(glm::ivec2& pos)
{
    auto& showDescription = *Status::instance().showDescription;
    controlPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    controlPanel_->setTheme(&headerTheme_);
    pos.x += controlPanel_->getWidth() + DELTA;

    sceneNameInput_ = controlPanel_->addTextInput("Scene " + std::to_string(showDescription.getSceneIndex() + 1));
    auto& show = Status::instance().show;
    sceneNameInput_->setText(show ? show->getCurrentScene()->getName() : "Enter scene name");
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
    midiChannelInput_->setText(std::to_string(showDescription.getMidiChannel()));
    midiChannelInput_->onTextInputEvent(this, &Gui::onMidiChannelInput);

    controlPanel_->addBreak();
    controlPanel_->addFRM()->setLabel("fps");
}

//--------------------------------------------------------------
void Gui::setupPlayPanel(glm::ivec2& pos, int w)
{
    playPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    playPanel_->setTheme(&commonTheme_);
    playPanel_->setWidth(w);
    pos.x += playPanel_->getWidth();
    auto* header = playPanel_->addLabel("Play");
    header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
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

//--------------------------------------------------------------
void Gui::setupMutePanel(glm::ivec2& pos, int w)
{
    mutePanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    mutePanel_->setTheme(&commonTheme_);
    mutePanel_->setWidth(w);
    pos.x += mutePanel_->getWidth();
    auto* header = mutePanel_->addLabel("Mute");
    header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
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

//--------------------------------------------------------------
void Gui::setupVideoFxPanel(glm::ivec2& pos)
{
    // Videos & FX panel
    videoFxPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    videoFxPanel_->setTheme(&commonTheme_);
    videoFxPanel_->setWidth(14 * DELTA);
    pos.x += videoFxPanel_->getWidth();
    auto* videoHeader = videoFxPanel_->addLabel("Video");
    videoHeader->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    videoFxPanel_->addBreak();

    for (auto i = 0; i < layerButtons_.size(); ++i) {
        layerButtons_[i] = videoFxPanel_->addButton({});
        layerButtons_[i]->onButtonEvent(this, &Gui::onLayerButton);
        layerButtons_[i]->setName(std::to_string(i));
    }

    videoFxPanel_->addBreak();
    auto* effectHeader = videoFxPanel_->addLabel("Effect");
    effectHeader->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    videoFxPanel_->addBreak();

    std::vector<string> options;
    for (auto i = 0u; i < MAX_EFFECTS; ++i) {
        if (i < static_cast<unsigned>(EffectType::Count))
            options.push_back(c_str(static_cast<EffectType>(i)));
    }

    for (auto i = 0; i < effectDropdowns_.size(); ++i) {
        effectDropdowns_[i] = videoFxPanel_->addDropdown("Select...", options);
        effectDropdowns_[i]->setName(std::to_string(i));
        effectDropdowns_[i]->select(i);
        effectDropdowns_[i]->onDropdownEvent(this, &Gui::onEffectDropdown);
    }
}

//--------------------------------------------------------------
void Gui::setupMidiPanel(glm::ivec2& pos, int w)
{
    midiPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    midiPanel_->setTheme(&commonTheme_);
    midiPanel_->setWidth(w);
    pos.x += midiPanel_->getWidth();
    auto* header = midiPanel_->addLabel("MIDI");
    header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
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

//--------------------------------------------------------------
void Gui::setupCcPanel(glm::ivec2& pos, int w)
{
    ccPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    ccPanel_->setTheme(&commonTheme_);
    ccPanel_->setWidth(w);
    pos.x += ccPanel_->getWidth();
    auto* header = ccPanel_->addLabel("CC");
    header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
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

//--------------------------------------------------------------
void Gui::setupAlphaPanel(glm::ivec2& pos, int w)
{
    alphaPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    alphaPanel_->setTheme(&commonTheme_);
    alphaPanel_->setWidth(w);
    pos.x += alphaPanel_->getWidth();
    auto* alphaHeader = alphaPanel_->addLabel("Alpha");
    alphaHeader->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    alphaPanel_->addBreak();
    for (auto i = 0; i < layerAlphaInputs_.size(); ++i) {
        layerAlphaInputs_[i] = alphaPanel_->addTextInput("");
        layerAlphaInputs_[i]->onTextInputEvent(this, &Gui::onLayerAlphaInput);
        layerAlphaInputs_[i]->setName(std::to_string(i));
    }

    alphaPanel_->addBreak();
    auto* paraHeader = alphaPanel_->addLabel("Param");
    paraHeader->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    alphaPanel_->addBreak();

    for (auto i = 0; i < effectParamInputs_.size(); ++i) {
        effectParamInputs_[i] = alphaPanel_->addTextInput("");
        effectParamInputs_[i]->onTextInputEvent(this, &Gui::onEffectParamInput);
        effectParamInputs_[i]->setName(std::to_string(i));
    }
}

//--------------------------------------------------------------
void Gui::setupRetriggerPanel(glm::ivec2 & pos)
{
    retriggerPanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    retriggerPanel_->setTheme(&commonTheme_);
    retriggerPanel_->setWidth(2.5 * DELTA);
    pos.x += retriggerPanel_->getWidth();
    auto* header = retriggerPanel_->addLabel("Re");
    header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    retriggerPanel_->addBreak();
    for (auto i = 0; i < layerRetriggerToggles_.size(); ++i) {
        layerRetriggerToggles_[i] = retriggerPanel_->addToggle({});
        layerRetriggerToggles_[i]->onToggleEvent(this, &Gui::onLayerRetriggerToggle);
        layerRetriggerToggles_[i]->setName(std::to_string(i));
    }

    retriggerPanel_->addBreak();
}

//--------------------------------------------------------------
void Gui::setupBlendModePanel(glm::ivec2& pos)
{
    blendModePanel_ = std::make_unique<ofxDatGui>(pos.x, pos.y);
    blendModePanel_->setTheme(&commonTheme_);
    blendModePanel_->setWidth(6 * DELTA);
    pos.x += blendModePanel_->getWidth();
    auto* header = blendModePanel_->addLabel("Blending Mode");
    header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
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

//--------------------------------------------------------------
void Gui::setupMidiDevicePanel(glm::ivec2& pos /*= glm::ivec2{}*/)
{
  pos.x += DELTA;
  pos.y += (MAX_LAYERS + 1) * (commonTheme_.layout.height + commonTheme_.layout.vMargin) + 2 * commonTheme_.layout.breakHeight + DELTA;
  static const auto finalPos = pos; // "save" the pos calculated in first setup

  midiDevicePanel_ = std::make_unique<ofxDatGui>(finalPos.x, finalPos.y);
  midiDevicePanel_->setAutoDraw(false);

  auto* header = midiDevicePanel_->addLabel("MIDI Input Devices");
  header->setLabelAlignment(ofxDatGuiAlignment::CENTER);
  midiDevicePanel_->addBreak();

  const auto devices = getStatus().midi->getPorts();
  for (const auto& device : devices) {
    auto* toggle = midiDevicePanel_->addToggle(device.name);
    toggle->setChecked(device.open);
    toggle->onToggleEvent(this, &Gui::onMidiDeviceToggle);
  }

  midiDevicePanel_->addBreak();
  auto* monitorToggle = midiDevicePanel_->addToggle("Monitor");
  monitorToggle->onToggleEvent(this, &Gui::onMidiMonitorToggle);
  monitorToggle->setChecked(midiMonitor_.on_);
  monitorToggle->setLabelAlignment(ofxDatGuiAlignment::CENTER);
  setupMidiMonitorLabel();
}

//--------------------------------------------------------------
void Gui::setupMidiMonitorLabel()
{
	if (midiMonitor_.on_)
	{
		midiMonitorLabel_ = midiDevicePanel_->addLabel("");
		midiMonitorLabel_->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	}
	else
	{
    // ofxDatGui does not provide removing components
    if (midiMonitorLabel_ != nullptr)
      midiMonitorLabel_->setLabel("");
		midiMonitorLabel_ = nullptr;
	}
}

//--------------------------------------------------------------
void Gui::threadedFunction()
{
  while (isThreadRunning()) {
    midiDevicesTimer_.waitNext();
    shouldUpdateDevices_ = true;
  }
}

} // namespace skinny
