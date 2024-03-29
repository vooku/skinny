#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Show.h"
#include "VideoSelector.h"
#include "FileSelector.h"
#include "MidiMonitor.h"
#include <array>
#include <filesystem>
#include <chrono>
#include <memory>

namespace skinny {

class Gui : public ofBaseApp, 
            public std::enable_shared_from_this<Gui>,
            public ofThread {
public:
    static const ofColor BACKGROUND_COLOR;

    void setup() override;
    void draw() override;
    void update() override;
    void exit() override;
    
    void reload();

    void setActiveLayer(int idx, bool active);
    void setActiveEffect(int idx, bool active);

    void resetJumpToIndex();

    bool requiresVisualMonitor() const;

    /**
     * @brief Display a single line message in the GUI.
     *
     * @param msg       Desired message
     * @param duration  Duration in ms
     */
    void displayMessage(const std::string& msg, int duration = 1000);

    void onLayerButton(ofxDatGuiButtonEvent e);
		void onControlButton(ofxDatGuiButtonEvent e);
		void onJumpToInput(ofxDatGuiTextInputEvent e);

    void onLayerMidiInput(ofxDatGuiTextInputEvent e);
    void onLayerCcInput(ofxDatGuiTextInputEvent e);
    void onLayerAlphaInput(ofxDatGuiTextInputEvent e);
    void onMasterAlphaCcInput(ofxDatGuiTextInputEvent e);
    void onEffectMidiInput(ofxDatGuiTextInputEvent e);
    void onEffectCcInput(ofxDatGuiTextInputEvent e);
    void onEffectParamInput(ofxDatGuiTextInputEvent e);
    void onSceneNameInput(ofxDatGuiTextInputEvent e);
    void onMidiChannelInput(ofxDatGuiTextInputEvent e);

    void onBlendModeDropdown(ofxDatGuiDropdownEvent e);
    void onEffectDropdown(ofxDatGuiDropdownEvent e);

    void onLayerPlayToggle(ofxDatGuiToggleEvent e);
    void onEffectPlayToggle(ofxDatGuiToggleEvent e);
    void onLayerMuteToggle(ofxDatGuiToggleEvent e);
    void onEffectMuteToggle(ofxDatGuiToggleEvent e);
    void onLayerRetriggerToggle(ofxDatGuiToggleEvent e);
    void onMidiDeviceToggle(ofxDatGuiToggleEvent e);
    void onMidiMonitorToggle(ofxDatGuiToggleEvent e);
    void onVisualMonitorToggle(ofxDatGuiToggleEvent e);
    void onSpoutToggle(ofxDatGuiToggleEvent e);

private:
    static const int MAX_CHARS = 20;
    static const int DELTA = 25;

    // control button names
    struct Btn {
        static const std::string NEXT;
        static const std::string PREV;
        static const std::string NEW;
        static const std::string SAVE;
        static const std::string SAVE_AS;
        static const std::string LOAD;
    };

    class CommonTheme : public ofxDatGuiTheme {
    public:
        CommonTheme();
    } commonTheme_;

    class HeaderTheme : public CommonTheme {
    public:
        HeaderTheme();
    } headerTheme_;

    struct Fonts {
        static const int sizeRegular = 12;
        static const int sizeItalic = 14;
        ofTrueTypeFont regular, italic;
    } fonts_;

    void setupControlPanel(glm::ivec2& pos);
    void setupPlayPanel(glm::ivec2& pos, int w);
    void setupMutePanel(glm::ivec2& pos, int w);
    void setupVideoFxPanel(glm::ivec2& pos);
    void setupMidiPanel(glm::ivec2& pos, int w);
    void setupCcPanel(glm::ivec2& pos, int w);
    void setupAlphaPanel(glm::ivec2& pos, int w);
    void setupRetriggerPanel(glm::ivec2& pos);
    void setupBlendModePanel(glm::ivec2& pos);
    void setupMidiDevicePanel(glm::ivec2& pos);
    void setupMidiMonitorLabel();

    void threadedFunction() override;

    void addBlank(ofxDatGui* panel);

    void save(std::filesystem::path path);

		MidiMonitor midiMonitor_;
    ofTimer midiDevicesTimer_;
    bool shouldUpdateDevices_ = false;
    bool shouldDrawVisualMonitor_ = true;

    std::unique_ptr<ofxDatGui> controlPanel_, playPanel_, mutePanel_,
                               videoFxPanel_, midiPanel_, ccPanel_,
                               alphaPanel_, retriggerPanel_, blendModePanel_,
                               midiDevicePanel_;
    
    // This class does not own any of the following pointers, do not try to delete them.
    ofxDatGuiTextInput* sceneNameInput_;
    ofxDatGuiTextInput* masterAlphaInput_;
    ofxDatGuiTextInput* midiChannelInput_;
    ofxDatGuiTextInput* jumpToInput_;
    std::vector<ofxDatGuiButton*> controlButtons_;
    std::array<ofxDatGuiButton*,    MAX_LAYERS> layerButtons_;
    std::array<ofxDatGuiTextInput*, MAX_LAYERS> layerMidiInputs_;
    std::array<ofxDatGuiTextInput*, MAX_LAYERS> layerCCInputs_;
    std::array<ofxDatGuiTextInput*, MAX_LAYERS> layerAlphaInputs_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerPlayToggles_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerMuteToggles_;
    std::array<ofxDatGuiToggle*,    MAX_LAYERS> layerRetriggerToggles_;
    std::array<ofxDatGuiDropdown*,  MAX_LAYERS> blendModeDropdowns_;
    std::array<ofxDatGuiDropdown*,  MAX_EFFECTS> effectDropdowns_;
    std::array<ofxDatGuiTextInput*, MAX_EFFECTS> effectMidiInputs_;
    std::array<ofxDatGuiTextInput*, MAX_EFFECTS> effectCCInputs_;
    std::array<ofxDatGuiToggle*,    MAX_EFFECTS> effectPlayToggles_;
    std::array<ofxDatGuiToggle*,    MAX_EFFECTS> effectMuteToggles_;
    std::array<ofxDatGuiTextInput*, MAX_EFFECTS> effectParamInputs_;
    ofxDatGuiLabel* midiMonitorLabel_ = nullptr;
    ofxDatGuiToggle* spoutToggle_ = nullptr;

    std::filesystem::path configPath_;
    std::string configName_;

    struct Message {
        std::string msg;
        std::chrono::milliseconds duration;
        std::chrono::system_clock::time_point start;
    } msg_;

    std::unique_ptr<VideoSelector> videoSelector_;
    std::unique_ptr<FileSelector> fileSelector_;

		ofTexture subsampledTexture_;
};

} // namespace skinny
