#pragma once
#include <JuceHeader.h>

class AudioModel {
public:
    AudioModel() = default;
    
    // Audio state getters/setters
    float getCurrentInputLevel() const { return currentInputLevel; }
    void setCurrentInputLevel(float level);
    
    // Add more audio-related data as needed
    bool isPlaying() const { return playing; }
    void setPlaying(bool isPlaying);
    
    float getVolume() const { return volume; }
    void setVolume(float newVolume);
    
    // Observer pattern for model changes
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void inputLevelChanged(float newLevel) {}
        virtual void playStateChanged(bool isPlaying) {}
        virtual void volumeChanged(float newVolume) {}
    };
    
    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }

private:
    float currentInputLevel = 0.0f;
    bool playing = false;
    float volume = 1.0f;
    
    juce::ListenerList<Listener> listeners;
    
    void notifyInputLevelChanged() {
        listeners.call([this](Listener& l) { l.inputLevelChanged(currentInputLevel); });
    }
    
    void notifyPlayStateChanged() {
        listeners.call([this](Listener& l) { l.playStateChanged(playing); });
    }
    
    void notifyVolumeChanged() {
        listeners.call([this](Listener& l) { l.volumeChanged(volume); });
    }
};