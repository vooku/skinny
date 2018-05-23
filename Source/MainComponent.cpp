#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setWantsKeyboardFocus(true);
    
    videoReader_ = std::make_unique<FFmpegVideoReader>();
    videoReader_->addVideoListener(this);

    transportSource_ = std::make_unique<AudioTransportSource>();
    transportSource_->setSource(videoReader_.get(), 0, nullptr);

    videoComponent_ = std::make_unique<FFmpegVideoComponent>();
    videoComponent_->setVideoReader(videoReader_.get());
    addAndMakeVisible(videoComponent_.get());

    setAudioChannels(0, 2);
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
    //shutdownOpenGL();
}

//==============================================================================
//void MainComponent::initialise()
//{
//    // Initialise GL objects for rendering here.
//}
//
//void MainComponent::shutdown()
//{
//    // Free any GL objects created for rendering here.
//}
//
//void MainComponent::render()
//{
//    // This clears the context with a black background.
//    OpenGLHelpers::clear (Colours::pink);
//
//    // Add your rendering code here...
//}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (videoReader_) videoReader_->prepareToPlay(samplesPerBlockExpected, sampleRate);
    if (transportSource_) transportSource_->prepareToPlay(samplesPerBlockExpected, sampleRate);

    readBuffer_.setSize(2, samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    AudioSourceChannelInfo info = { &readBuffer_, bufferToFill.startSample, bufferToFill.numSamples };
    // the AudioTransportSource takes care of start, stop and resample
    transportSource_->getNextAudioBlock(info);
}

void MainComponent::releaseResources()
{
    transportSource_->releaseResources();
    videoReader_->releaseResources();
}

void MainComponent::paint (Graphics& g)
{

}

void MainComponent::resized()
{
    videoComponent_->setBounds(getBounds());
}

bool MainComponent::keyPressed(const KeyPress &key)
{
    if (key == KeyPress::escapeKey) {
        JUCEApplication::getInstance()->systemRequestedQuit();
        return true;
    }
    if (key.getTextCharacter() == 'l') {
        File video = File::getCurrentWorkingDirectory().getChildFile("./videos/RGB Random 1 small.mp4");
        videoReader_->loadMovieFile(video);

        setSize(videoReader_->getVideoWidth(), videoReader_->getVideoHeight());

        transportSource_->setSource(videoReader_.get(), 0, nullptr, videoReader_->getVideoSamplingRate(), videoReader_->getVideoChannels());
        readBuffer_.setSize(videoReader_->getVideoChannels(), readBuffer_.getNumSamples());

        videoReader_->setNextReadPosition(0);
        videoReader_->setLooping(true);
        
        if (AudioIODevice* device = deviceManager.getCurrentAudioDevice()) {
            videoReader_->prepareToPlay(device->getCurrentBufferSizeSamples(), device->getCurrentSampleRate());
            readBuffer_.setSize(videoReader_->getVideoChannels(), device->getCurrentBufferSizeSamples());

        }
        
        DBG("Channels:          " + String(videoReader_->getVideoChannels()));
        DBG("Duration (sec):    " + String(videoReader_->getVideoDuration()));
        DBG("Framerate (1/sec): " + String(videoReader_->getFramesPerSecond()));
        DBG("SampleFormat:      " + String(av_get_sample_fmt_name(videoReader_->getSampleFormat())));
        DBG("Width:             " + String(videoReader_->getVideoWidth()));
        DBG("Height:            " + String(videoReader_->getVideoHeight()));
        DBG("Pixel format:      " + String(av_get_pix_fmt_name(videoReader_->getPixelFormat())));
        DBG("Pixel aspect ratio:" + String(videoReader_->getVideoPixelAspect()));

        return true;
    }
    if (key.getTextCharacter() == 'p') {
        transportSource_->start();
        return true;
    }
    return false;
}