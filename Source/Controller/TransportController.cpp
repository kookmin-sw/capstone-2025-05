#include "TransportController.h"

TransportController::TransportController(AudioModel& model)
    : audioModel(model)
{
    formatManager.registerBasicFormats();
}

void TransportController::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void TransportController::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void TransportController::releaseResources()
{
    transportSource.releaseResources();
}

void TransportController::startPlayback()
{
    transportSource.start();
    audioModel.setPlaying(true);
}

void TransportController::stopPlayback()
{
    transportSource.stop();
    audioModel.setPlaying(false);
}

void TransportController::loadFile(const juce::File& audioFile)
{
    auto* reader = formatManager.createReaderFor(audioFile);
    
    if (reader != nullptr)
    {
        double readerSampleRate = reader->sampleRate;
        
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, readerSampleRate);
        readerSource = std::move(newSource);
    }
}