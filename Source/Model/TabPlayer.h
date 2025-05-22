#pragma once
#include <JuceHeader.h>
#include "gp_parser.h"
#include <memory>

class TabPlayer : public juce::AudioProcessor, public juce::AudioIODeviceCallback
{
public:
    TabPlayer();
    ~TabPlayer() override {}

    void setTabFile(const gp_parser::TabFile& file);
    void startPlaying();
    void stopPlaying();
    bool isPlaying() const { return playing; }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void releaseResources() override;

    const juce::String getName() const override { return "TabPlayer"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}
    bool hasEditor() const override { return false; }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    
    void getStateInformation(juce::MemoryBlock& destData) override {}
    void setStateInformation(const void* data, int sizeInBytes) override {}

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                         int numInputChannels,
                                         float* const* outputChannelData,
                                         int numOutputChannels,
                                         int numSamples,
                                         const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    void setPlaybackPosition(int track, int measure, int beat);
    int getCurrentMeasure() const { return currentMeasure; }
    int getCurrentTrack() const { return currentTrack; }
    int getCurrentBeat() const { return currentBeat; }
    const std::unique_ptr<gp_parser::TabFile>& getTabFile() const { return tabFile; }

private:
    void stopAllActiveNotes(juce::MidiBuffer& midiMessages);
    
    std::unique_ptr<gp_parser::TabFile> tabFile;
    
    bool playing = false;
    double currentSampleRate = 44100.0;
    int samplesPerBlock = 512;
    bool trackEndReached = false;
    int silenceCountdown = 0;
    int currentTrack = 0;
    int currentMeasure = 0;
    int currentBeat = 0;
    int currentTick = 0;
    double samplesPerTick = 0.0;
    double sampleCounter = 0.0;
    double tickCounter = 0.0;
    int ticksPerBeat = 960;
    
    struct ActiveNote {
        int midiNote;
        int channel;
        int startSample;
        int duration;
        bool isPlaying;
        ActiveNote(int note, int ch, int start, int dur) 
            : midiNote(note), channel(ch), startSample(start), 
              duration(dur), isPlaying(true) {}
    };
    
    std::vector<ActiveNote> activeNotes;
    
    void processNextBeat(juce::MidiBuffer& midiMessages, int startSample);
    double getBPM() const;
    void calculateTickSamples();
    void processActiveNotes(juce::MidiBuffer& midiMessages, int numSamples);
    int stringToMidiNote(int string, int fret);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabPlayer)
};