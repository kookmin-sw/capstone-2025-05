#include "GuitarPracticeComponent.h"
#include "MainComponent.h"
#include "Controller/GuitarPracticeController.h"

#include "View/TopBar.h"
#include "View/CenterPanel.h"
#include "View/LeftPanel.h"
#include "View/RightPanel.h"

#include "EnvLoader.h"

class RecordingThumbnail : public juce::Component,
                          private juce::ChangeListener
{
public:
    RecordingThumbnail()
    {
        formatManager.registerBasicFormats();
        thumbnail.addChangeListener(this);
    }

    ~RecordingThumbnail() override
    {
        thumbnail.removeChangeListener(this);
    }

    juce::AudioThumbnail& getAudioThumbnail() { return thumbnail; }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::lightgrey);

        if (thumbnail.getTotalLength() > 0.0)
        {
            auto endTime = thumbnail.getTotalLength();
            auto thumbArea = getLocalBounds();
            thumbnail.drawChannels(g, thumbArea.reduced(2), 0.0, endTime, 1.0f);
        }
        else
        {
            g.setFont(14.0f);
            g.drawFittedText("(No recorded file)", getLocalBounds(), juce::Justification::centred, 2);
        }
    }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache { 10 };
    juce::AudioThumbnail thumbnail { 512, formatManager, thumbnailCache };

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            repaint();
    }
};

class AudioRecorder : public juce::AudioIODeviceCallback
{
public:
    AudioRecorder(juce::AudioThumbnail& thumbnailToUpdate)
        : thumbnail(thumbnailToUpdate)
    {
        backgroundThread.startThread();
    }

    ~AudioRecorder() override
    {
        stop();
    }

    void startRecording(const juce::File& file)
    {
        stop();

        if (sampleRate > 0)
        {
            file.deleteFile();

            if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream()))
            {
                juce::WavAudioFormat wavFormat;

                if (auto writer = wavFormat.createWriterFor(fileStream.get(), sampleRate, 1, 16, {}, 0))
                {
                    fileStream.release();

                    threadedWriter.reset(new juce::AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768));

                    thumbnail.reset(writer->getNumChannels(), writer->getSampleRate());
                    nextSampleNum = 0;

                    const juce::ScopedLock sl(writerLock);
                    activeWriter = threadedWriter.get();
                }
            }
        }
    }

    void stop()
    {
        {
            const juce::ScopedLock sl(writerLock);
            activeWriter = nullptr;
        }

        threadedWriter.reset();
    }

    bool isRecording() const
    {
        return activeWriter.load() != nullptr;
    }

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override
    {
        sampleRate = device->getCurrentSampleRate();
    }

    void audioDeviceStopped() override
    {
        sampleRate = 0;
    }

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                         float* const* outputChannelData, int numOutputChannels,
                                         int numSamples, const juce::AudioIODeviceCallbackContext& context) override
    {
        const juce::ScopedLock sl(writerLock);

        if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels())
        {
            activeWriter.load()->write(inputChannelData, numSamples);

            juce::AudioBuffer<float> buffer(const_cast<float**>(inputChannelData), thumbnail.getNumChannels(), numSamples);
            thumbnail.addBlock(nextSampleNum, buffer, 0, numSamples);
            nextSampleNum += numSamples;
        }

        for (int i = 0; i < numOutputChannels; ++i)
            if (outputChannelData[i] != nullptr)
                juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
    }

private:
    juce::AudioThumbnail& thumbnail;
    juce::TimeSliceThread backgroundThread { "Audio Recorder Thread" };
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    double sampleRate = 0.0;
    juce::int64 nextSampleNum = 0;

    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};

GuitarPracticeComponent::GuitarPracticeComponent(MainComponent &mainComp)
    : mainComponent(mainComp), deviceManager(mainComp.getDeviceManager())
{
    audioModel.addListener(this);
    controller = std::make_unique<GuitarPracticeController>(audioModel, deviceManager);
    controller->setView(this);
    
    playButton.setButtonText("Play");
    playButton.onClick = [this]() { togglePlayback(); };
    addAndMakeVisible(playButton);
    playButton.setEnabled(false);
    
    formatManager.registerBasicFormats();
    recordingThumbnail = std::make_unique<RecordingThumbnail>();
    audioRecorder = std::make_unique<AudioRecorder>(recordingThumbnail->getAudioThumbnail());
    
    recordButton.setButtonText("Record");
    recordButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff5c5c));
    recordButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    recordButton.onClick = [this]() {
        if (isRecording())
            stopRecording();
        else
            startRecording();
    };
    addAndMakeVisible(recordButton);
    recordButton.setEnabled(false);
    
    analyzeButton.setButtonText("Analyze");
    analyzeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff5c5cff));
    analyzeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    analyzeButton.onClick = [this]() { analyzeRecording(); };
    addAndMakeVisible(analyzeButton);
    analyzeButton.setEnabled(false);

    deviceManager.addAudioCallback(audioRecorder.get());
    
    topBar = std::make_unique<TopBar>(*this);
    centerPanel = std::make_unique<CenterPanel>();
    leftPanel = std::make_unique<LeftPanel>(audioModel);
    rightPanel = std::make_unique<RightPanel>();
    
    try {
        scoreComponent = std::make_unique<ScoreComponent>(controller->getTabPlayer());
        addAndMakeVisible(scoreComponent.get());
    }
    catch (const std::exception& e) {
        DBG("Error creating ScoreComponent: " + juce::String(e.what()));
    }

    addAndMakeVisible(topBar.get());
    addAndMakeVisible(centerPanel.get());
    addAndMakeVisible(leftPanel.get());
    addAndMakeVisible(rightPanel.get());
    addAndMakeVisible(recordingThumbnail.get());
}

GuitarPracticeComponent::~GuitarPracticeComponent()
{
    audioModel.removeListener(this);
    
    if (audioRecorder)
        deviceManager.removeAudioCallback(audioRecorder.get());
}

void GuitarPracticeComponent::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void GuitarPracticeComponent::resized()
{
    auto bounds = getLocalBounds();
    auto topBarHeight = 50;
    auto controlsHeight = 40;
    
    topBar->setBounds(bounds.removeFromTop(topBarHeight));
    
    auto scoreHeight = 300;
    auto scoreArea = bounds.removeFromBottom(scoreHeight);
    scoreComponent->setBounds(scoreArea);
    
    auto controlsArea = bounds.removeFromBottom(controlsHeight);
    auto buttonWidth = 120;
    auto spacing = 10;
    
    playButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    controlsArea.removeFromLeft(spacing);
    recordButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    controlsArea.removeFromLeft(spacing);
    analyzeButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    
    auto waveformHeight = 100;
    auto waveformArea = bounds.removeFromBottom(waveformHeight);
    recordingThumbnail->setBounds(waveformArea);
    
    leftPanel->setBounds(bounds.removeFromLeft(300));
    rightPanel->setBounds(bounds.removeFromRight(300));
    
    centerPanel->setBounds(bounds.reduced(50));
}

void GuitarPracticeComponent::playStateChanged(bool isPlaying)
{
    updatePlaybackState(isPlaying);
}

void GuitarPracticeComponent::volumeChanged(float newVolume)
{
    updateVolumeDisplay(newVolume);
}

void GuitarPracticeComponent::updatePlaybackState(bool isNowPlaying)
{
    if (isNowPlaying)
    {
        playButton.setButtonText("Stop");
    }
    else
    {
        playButton.setButtonText("Play");
    }
}

void GuitarPracticeComponent::updateVolumeDisplay(float volume)
{
}

void GuitarPracticeComponent::togglePlayback()
{
    controller->togglePlayback();
}

void GuitarPracticeComponent::loadSong(const juce::String& songId)
{
    bool success = controller->loadSong(songId);
    
    if (success) {
        playButton.setEnabled(true);
        recordButton.setEnabled(true);
        
        if (scoreComponent != nullptr) {
            try {
                scoreComponent->updateScore();
            }
            catch (const std::exception& e) {
                DBG("Error updating score component: " + juce::String(e.what()));
            }
        }
    }
}

void GuitarPracticeComponent::updateUI()
{
}

void GuitarPracticeComponent::startRecording()
{
    if (audioModel.isPlaying())
        controller->togglePlayback();
    
    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    lastRecording = parentDir.getNonexistentChildFile("Recording", ".wav");
    
    audioRecorder->startRecording(lastRecording);
    
    recordButton.setButtonText("Stop");
    analyzeButton.setEnabled(false);
    
    DBG("Recording started: " + lastRecording.getFullPathName());
}

void GuitarPracticeComponent::stopRecording()
{
    audioRecorder->stop();
    
    recordButton.setButtonText("Record");
    analyzeButton.setEnabled(true);
    
    DBG("Recording finished: " + lastRecording.getFullPathName());
}

bool GuitarPracticeComponent::isRecording() const
{
    return audioRecorder && audioRecorder->isRecording();
}

void GuitarPracticeComponent::analyzeRecording()
{
    if (lastRecording.existsAsFile()) {
        controller->analyzeRecording(lastRecording);
    } else {
        DBG("No recorded file to analyze.");
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                              "Analysis Error",
                                              "No recording file found. Please record audio first.",
                                              "OK");
    }
}