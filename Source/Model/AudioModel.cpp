#include "AudioModel.h"

void AudioModel::setCurrentInputLevel(float level)
{
    if (currentInputLevel != level)
    {
        currentInputLevel = level;
        notifyInputLevelChanged();
    }
}

void AudioModel::setPlaying(bool isPlaying)
{
    if (playing != isPlaying)
    {
        playing = isPlaying;
        notifyPlayStateChanged();
    }
}

void AudioModel::setVolume(float newVolume)
{
    if (volume != newVolume)
    {
        volume = newVolume;
        notifyVolumeChanged();
    }
}

void AudioModel::setCurrentPosition(double positionInSeconds)
{
    if (currentPositionInSeconds != positionInSeconds)
    {
        currentPositionInSeconds = positionInSeconds;
        notifyPositionChanged();
    }
}

// Private 알림 메서드 구현 - 비동기 호출로 변경

void AudioModel::notifyPlayStateChanged()
{
    // 상태값 캡처
    bool currentState = playing;
    
    // 메인 스레드에서 모든 리스너에 비동기 알림
    juce::MessageManager::callAsync([this, currentState]() {
        // 기존 리스너에게 알림
        listeners.call([currentState](Listener& l) { l.playStateChanged(currentState); });
        
        // 새 인터페이스 리스너에게 알림
        modelListeners.call([currentState](IAudioModelListener& l) { l.onPlayStateChanged(currentState); });
    });
}

void AudioModel::notifyVolumeChanged()
{
    // 상태값 캡처
    float currentVolume = volume;
    
    // 메인 스레드에서 모든 리스너에 비동기 알림
    juce::MessageManager::callAsync([this, currentVolume]() {
        // 기존 리스너에게 알림
        listeners.call([currentVolume](Listener& l) { l.volumeChanged(currentVolume); });
        
        // 새 인터페이스 리스너에게 알림
        modelListeners.call([currentVolume](IAudioModelListener& l) { l.onVolumeChanged(currentVolume); });
    });
}

void AudioModel::notifyPositionChanged()
{
    // 상태값 캡처
    double currentPosition = currentPositionInSeconds;
    
    // 메인 스레드에서 모든 리스너에 비동기 알림
    juce::MessageManager::callAsync([this, currentPosition]() {
        // 새 인터페이스 리스너에게만 알림 (기존 리스너는 이 기능 지원 안함)
        modelListeners.call([currentPosition](IAudioModelListener& l) { l.onPositionChanged(currentPosition); });
    });
}

void AudioModel::notifyInputLevelChanged()
{
    // 상태값 캡처
    float currentLevel = currentInputLevel;
    
    // 메인 스레드에서 모든 리스너에 비동기 알림
    juce::MessageManager::callAsync([this, currentLevel]() {
        // 기존 리스너에게 알림
        listeners.call([currentLevel](Listener& l) { l.inputLevelChanged(currentLevel); });
        
        // 새 인터페이스 리스너에게 알림
        modelListeners.call([currentLevel](IAudioModelListener& l) { l.onInputLevelChanged(currentLevel); });
    });
}