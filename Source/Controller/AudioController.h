#pragma once
#include <JuceHeader.h>
#include "../Model/AudioModel.h"

class AudioController : public juce::AudioIODeviceCallback {
public:
    AudioController(AudioModel& model, juce::AudioDeviceManager& deviceManager);
    ~AudioController() override;
    
    // AudioIODeviceCallback implementation
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, 
                                         int numInputChannels,
                                         float* const* outputChannelData, 
                                         int numOutputChannels,
                                         int numSamples,
                                         const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    
private:
    AudioModel& audioModel;
    juce::AudioDeviceManager& deviceManager;
};