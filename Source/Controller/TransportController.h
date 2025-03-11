#pragma once
#include <JuceHeader.h>
#include "../Model/AudioModel.h"

class TransportController {
public:
    TransportController(AudioModel& model);
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    
    void startPlayback();
    void stopPlayback();
    void loadFile(const juce::File& audioFile);
    
private:
    AudioModel& audioModel;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioFormatManager formatManager;
};