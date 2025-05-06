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
        virtual void playbackStateChanged(bool isPlaying) {}
        virtual void volumeChanged(float newVolume) {}
        virtual void playbackPositionChanged(double positionInSeconds) {}
    };
    
    // Legacy Listener 관리
    void addListener(Listener* l) { 
        const juce::ScopedLock lock(listenerMutex); 
        legacyListeners.add(l); 
    }
    
    void removeListener(Listener* l) { 
        const juce::ScopedLock lock(listenerMutex); 
        legacyListeners.remove(l); 
    }
    
    // 새로운 리스너 인터페이스 관리
    void addListener(IAudioModelListener* l) { 
        const juce::ScopedLock lock(listenerMutex); 
        listeners.add(l); 
    }
    
    void removeListener(IAudioModelListener* l) { 
        const juce::ScopedLock lock(listenerMutex); 
        listeners.remove(l); 
    }
    
private:
    // Audio state
    std::atomic<float> currentInputLevel { 0.0f };
    std::atomic<bool> playing { false };
    std::atomic<float> volume { 1.0f };
    std::atomic<double> currentPositionInSeconds { 0.0 };
    
    // 리스너 관리
    juce::CriticalSection listenerMutex;
    juce::ListenerList<Listener> legacyListeners;
    juce::ListenerList<IAudioModelListener> listeners;
    
    // 상태 변경 알림 메서드
    void notifyInputLevelChanged();
    void notifyPlayStateChanged();
    void notifyVolumeChanged();
    void notifyPositionChanged();
    
    // 안전한 리스너 호출 도우미 메서드
    template<typename ListenerType, typename Callback>
    void safelyCallListeners(juce::ListenerList<ListenerType>& listenerList, Callback&& callback)
    {
        // 리스너 목록 복사본을 만들어 작업
        const juce::ScopedLock lock(listenerMutex);
        
        // 복사 대신 직접 Iterator를 사용하여 리스너 호출
        listenerList.call(std::forward<Callback>(callback));
    }
};