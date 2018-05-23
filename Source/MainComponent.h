#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : /*public OpenGLAppComponent,*/
                      public AudioAppComponent,
                      public FFmpegVideoListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    //void initialise() override;
    //void shutdown() override;
    //void render() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (Graphics& g) override;
    void resized() override;
    bool keyPressed(const KeyPress &key) override;

private:
    std::unique_ptr<FFmpegVideoReader> videoReader_;
    std::unique_ptr<FFmpegVideoComponent> videoComponent_;
    std::unique_ptr<AudioTransportSource> transportSource_;
    AudioSampleBuffer readBuffer_;
    double videoAspectRatio_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
