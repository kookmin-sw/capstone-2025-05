#pragma once
#include <JuceHeader.h>
#include "IAudioModelListener.h"

class AudioModel {
public:
    AudioModel() = default;
    
    // Audio state getters/setters
    float getCurrentInputLevel() const { return currentInputLevel.load(); }
    void setCurrentInputLevel(float level);
    
    // Add more audio-related data as needed
    bool isPlaying() const { return playing.load(); }
    void setPlaying(bool isPlaying);
    
    float getVolume() const { return volume.load(); }
    void setVolume(float newVolume);
    
    double getCurrentPosition() const { return currentPositionInSeconds.load(); }
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
    // 스레드 안전을 위해 atomic 타입 사용
    std::atomic<float> currentInputLevel{0.0f};
    std::atomic<bool> playing{false};
    std::atomic<float> volume{1.0f};
    std::atomic<double> currentPositionInSeconds{0.0};
    
    juce::ThreadSafeListenerList<Listener> listeners;
    juce::ThreadSafeListenerList<IAudioModelListener> modelListeners;
    
    // 비동기 알림 메서드 - AudioModel.cpp에서 구현
    void notifyInputLevelChanged();
    void notifyPlayStateChanged();
    void notifyVolumeChanged();
    void notifyPositionChanged();
};