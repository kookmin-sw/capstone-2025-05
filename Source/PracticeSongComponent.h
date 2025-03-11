#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"
#include "Controller/AudioController.h"
#include "Controller/TransportController.h"
#include "View/TopBar.h"
#include "View/CenterPanel.h"
#include "View/LeftPanel.h"
#include "View/RightPanel.h"
#include "View/EffectControls.h"
#include "View/WaveformGraph.h"
#include "AudioPlaying.h"

class MainComponent;  // 전방 선언

class PracticeSongComponent : public juce::Component {
public:
    PracticeSongComponent(MainComponent& mainComponent);
    ~PracticeSongComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Transport-related methods
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    
    // Access to model and controllers
    AudioModel& getAudioModel() { return audioModel; }
    TransportController& getTransportController() { return *transportController; }
    
private:
    MainComponent& mainComponent;
    juce::AudioDeviceManager& deviceManager;
    
    // MVC components
    AudioModel audioModel;
    std::unique_ptr<AudioController> audioController;
    std::unique_ptr<TransportController> transportController;
    
    // View components
    std::unique_ptr<TopBar> topBar;
    std::unique_ptr<CenterPanel> centerPanel;
    std::unique_ptr<LeftPanel> leftPanel;
    std::unique_ptr<RightPanel> rightPanel;
    std::unique_ptr<EffectControls> effectControls;
    std::unique_ptr<AudioPlaybackDemo> waveformGraph;
    
    juce::AudioTransportSource transport;
};