#include "PracticeSongComponent.h"
#include "MainComponent.h"
#include "View/TopBar.h"
#include "View/CenterPanel.h"
#include "View/LeftPanel.h"
#include "View/RightPanel.h"
#include "View/EffectControls.h"

PracticeSongComponent::PracticeSongComponent(MainComponent &mainComp)
    : mainComponent(mainComp), deviceManager(mainComp.getDeviceManager()), parser("D:/audio_dataset/recording/homecoming/homecoming.gp5")
{
    // Initialize model first
    try
    {
        player.setTabFile(parser.getTabFile());
        DBG("Tab file set successfully");
    }
    catch (const std::exception& e)
    {
        DBG("Failed to load tab file: " + juce::String(e.what()));
        return;
    }

    auto result = deviceManager.initialiseWithDefaultDevices(0, 2);
    if (result.isEmpty())
        DBG("Audio device initialized successfully!");
    else
        DBG("Audio device initialization failed: " + result);

    // setAudioChannels(0, 2);

    playButton.setButtonText("Play");
    playButton.onClick = [this]() { togglePlayback(); };
    addAndMakeVisible(playButton);

    // Create controllers
    audioController = std::make_unique<AudioController>(audioModel, deviceManager);
    transportController = std::make_unique<TransportController>(audioModel);

    // Create view components
    topBar = std::make_unique<TopBar>(*this);
    centerPanel = std::make_unique<CenterPanel>();
    leftPanel = std::make_unique<LeftPanel>(audioModel); // Pass model to view
    rightPanel = std::make_unique<RightPanel>();
    // effectControls = std::make_unique<EffectControls>();
    // waveformGraph = std::make_unique<AudioPlaybackDemo>();
    scoreComponent = std::make_unique<ScoreComponent>(player);

    // Add components to view
    addAndMakeVisible(topBar.get());
    addAndMakeVisible(centerPanel.get());
    addAndMakeVisible(leftPanel.get());
    addAndMakeVisible(rightPanel.get());
    // addAndMakeVisible(effectControls.get());
    // addAndMakeVisible(waveformGraph.get());
    addAndMakeVisible(scoreComponent.get());
}

PracticeSongComponent::~PracticeSongComponent()
{
    // Controllers will clean up their resources in their destructors
}

void PracticeSongComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportController->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PracticeSongComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    transportController->getNextAudioBlock(bufferToFill);
}

void PracticeSongComponent::releaseResources()
{
    transportController->releaseResources();
}

void PracticeSongComponent::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PracticeSongComponent::resized()
{
    auto bounds = getLocalBounds();
    topBar->setBounds(bounds.removeFromTop(50));
    scoreComponent->setBounds(bounds.removeFromBottom(300));
    // waveformGraph->setBounds(bounds.removeFromBottom(300));
    // effectControls->setBounds(bounds.removeFromBottom(100));
    leftPanel->setBounds(bounds.removeFromLeft(300));
    rightPanel->setBounds(bounds.removeFromRight(300));
    centerPanel->setBounds(bounds.reduced(50));
}

void PracticeSongComponent::togglePlayback()
{
    if (!isPlaying)
    {
        player.startPlaying();
        playButton.setButtonText("Stop");
        isPlaying = true;
        DBG("Playback started");
    }
    else
    {
        player.stopPlaying();
        playButton.setButtonText("Play");
        isPlaying = false;
        DBG("Playback stopped");
    }
}