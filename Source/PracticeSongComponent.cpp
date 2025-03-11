#include "PracticeSongComponent.h"
#include "MainComponent.h"
#include "View/TopBar.h"
#include "View/CenterPanel.h"
#include "View/LeftPanel.h"
#include "View/RightPanel.h"
#include "View/EffectControls.h"

PracticeSongComponent::PracticeSongComponent(MainComponent &mainComp)
    : mainComponent(mainComp), deviceManager(mainComp.getDeviceManager())
{
    // Initialize model first

    // Create controllers
    audioController = std::make_unique<AudioController>(audioModel, deviceManager);
    transportController = std::make_unique<TransportController>(audioModel);

    // Create view components
    topBar = std::make_unique<TopBar>(*this);
    centerPanel = std::make_unique<CenterPanel>();
    leftPanel = std::make_unique<LeftPanel>(audioModel); // Pass model to view
    rightPanel = std::make_unique<RightPanel>();
    effectControls = std::make_unique<EffectControls>();
    waveformGraph = std::make_unique<AudioPlaybackDemo>();

    // Add components to view
    addAndMakeVisible(topBar.get());
    addAndMakeVisible(centerPanel.get());
    addAndMakeVisible(leftPanel.get());
    addAndMakeVisible(rightPanel.get());
    addAndMakeVisible(effectControls.get());
    addAndMakeVisible(waveformGraph.get());
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
    waveformGraph->setBounds(bounds.removeFromBottom(300));
    effectControls->setBounds(bounds.removeFromBottom(100));
    leftPanel->setBounds(bounds.removeFromLeft(300));
    rightPanel->setBounds(bounds.removeFromRight(300));
    centerPanel->setBounds(bounds.reduced(50));
}