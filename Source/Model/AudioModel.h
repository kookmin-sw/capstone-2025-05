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
    
    // 오디오 버퍼 관련 메서드 - 실시간 스레드 안전성 개선
    bool hasLatestAudioBuffer() const { return latestAudioBuffer.getNumSamples() > 0; }
    
    // 버퍼를 복사본으로 반환
    juce::AudioBuffer<float> getLatestAudioBuffer() const { 
        const juce::ScopedLock sl(bufferLock);
        return latestAudioBuffer; 
    }
    
    // 버퍼 설정 시 복사본 저장 (참조 방식 피함)
    void setLatestAudioBuffer(const juce::AudioBuffer<float>& buffer) {
        const juce::ScopedLock sl(bufferLock);
        latestAudioBuffer.makeCopyOf(buffer);
    }
    
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
        legacyListeners.removeFirstMatchingValue(l); 
    }
    
    // 새로운 리스너 인터페이스 관리
    void addListener(IAudioModelListener* l) { 
        const juce::ScopedLock lock(listenerMutex); 
        listeners.add(l); 
    }
    
    void removeListener(IAudioModelListener* l) { 
        const juce::ScopedLock lock(listenerMutex); 
        listeners.removeFirstMatchingValue(l); 
    }
    
private:
    // 상태 변경 알림 메서드
    void notifyInputLevelChanged();
    void notifyPlayStateChanged();
    void notifyVolumeChanged();
    void notifyPositionChanged();
    
    // 안전한 리스너 호출 템플릿 메서드
    template <typename ListenerType, typename CallbackType>
    void safelyCallListeners(juce::Array<ListenerType*>& listenerArray, CallbackType&& callback)
    {
        // 현재 리스너 목록의 복사본 생성
        juce::Array<ListenerType*> listenersCopy;
        {
            const juce::ScopedLock lock(listenerMutex);
            listenersCopy = listenerArray;
        }
        
        // 각 리스너에 대해 콜백 호출
        for (auto* listener : listenersCopy)
            if (listener != nullptr)
                callback(*listener);
    }
    
    // Audio state
    std::atomic<float> currentInputLevel { 0.0f };
    std::atomic<bool> playing { false };
    std::atomic<float> volume { 1.0f };
    std::atomic<double> currentPositionInSeconds { 0.0 };
    
    // 최신 오디오 버퍼 저장
    juce::AudioBuffer<float> latestAudioBuffer;
    mutable juce::CriticalSection bufferLock;
    
    // Listener collections
    juce::Array<Listener*> legacyListeners;
    juce::Array<IAudioModelListener*> listeners;
    juce::CriticalSection listenerMutex;
};