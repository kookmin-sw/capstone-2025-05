#pragma once
#include <JuceHeader.h>
#include "IAudioModelListener.h"

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
    
    double getCurrentPosition() const { return currentPositionInSeconds; }
    void setCurrentPosition(double positionInSeconds);
    
    // 기존 Listener 클래스는 레거시 지원을 위해 유지하되,
    // 새로운 리스너 인터페이스로 점진적으로 마이그레이션 권장
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void inputLevelChanged(float newLevel) {}
        virtual void playStateChanged(bool isPlaying) {}
        virtual void volumeChanged(float newVolume) {}
    };
    
    // 기존 Observer 패턴 메서드들 - 레거시 지원
    void addListener(Listener* listener) { listeners.add(listener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }
    
    // 새로운 Observer 패턴 메서드들 - 인터페이스 기반
    void addListener(IAudioModelListener* listener) { modelListeners.add(listener); }
    void removeListener(IAudioModelListener* listener) { modelListeners.remove(listener); }

private:
    float currentInputLevel = 0.0f;
    bool playing = false;
    float volume = 1.0f;
    double currentPositionInSeconds = 0.0;
    
    juce::ListenerList<Listener> listeners;
    juce::ListenerList<IAudioModelListener> modelListeners;
    
    void notifyInputLevelChanged() {
        // 기존 리스너에게 알림
        listeners.call([this](Listener& l) { l.inputLevelChanged(currentInputLevel); });
    }
    
    void notifyPlayStateChanged() {
        // 기존 리스너에게 알림
        listeners.call([this](Listener& l) { l.playStateChanged(playing); });
        
        // 새 인터페이스 리스너에게 알림
        modelListeners.call([this](IAudioModelListener& l) { l.onPlayStateChanged(playing); });
    }
    
    void notifyVolumeChanged() {
        // 기존 리스너에게 알림
        listeners.call([this](Listener& l) { l.volumeChanged(volume); });
        
        // 새 인터페이스 리스너에게 알림
        modelListeners.call([this](IAudioModelListener& l) { l.onVolumeChanged(volume); });
    }
    
    void notifyPositionChanged() {
        // 새 인터페이스 리스너에게만 알림 (기존 리스너는 이 기능 지원 안함)
        modelListeners.call([this](IAudioModelListener& l) { l.onPositionChanged(currentPositionInSeconds); });
    }
};