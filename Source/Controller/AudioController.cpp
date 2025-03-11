#include "AudioController.h"

AudioController::AudioController(AudioModel& model, juce::AudioDeviceManager& manager)
    : audioModel(model), deviceManager(manager)
{
    deviceManager.addAudioCallback(this);
}

AudioController::~AudioController()
{
    deviceManager.removeAudioCallback(this);
}

void AudioController::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                      int numInputChannels,
                                                      float* const* outputChannelData,
                                                      int numOutputChannels,
                                                      int numSamples,
                                                      const juce::AudioIODeviceCallbackContext& context)
{
    // Calculate input level
    if (numInputChannels > 0 && inputChannelData != nullptr)
    {
        float currentBlockLevel = 0.0f;
        
        for (int channel = 0; channel < numInputChannels; ++channel)
        {
            if (inputChannelData[channel] != nullptr)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    float sample = std::abs(inputChannelData[channel][i]);
                    currentBlockLevel = juce::jmax(currentBlockLevel, sample);
                }
            }
        }
        
        // Update model with smoothing (which will notify listeners)
        float newLevel = juce::jmax(audioModel.getCurrentInputLevel() * 0.7f, currentBlockLevel);
        audioModel.setCurrentInputLevel(newLevel);
    }
    
    // Clear output buffers
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (outputChannelData[channel] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    }
}

void AudioController::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    audioModel.setCurrentInputLevel(0.0f);
}

void AudioController::audioDeviceStopped()
{
    audioModel.setCurrentInputLevel(0.0f);
}