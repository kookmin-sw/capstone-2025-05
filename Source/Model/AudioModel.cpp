#include "AudioModel.h"

void AudioModel::setCurrentInputLevel(float level)
{
    if (currentInputLevel.load() != level)
    {
        currentInputLevel = level;
        notifyInputLevelChanged();
    }
}

void AudioModel::setPlaying(bool isPlaying)
{
    if (playing.load() != isPlaying)
    {
        playing = isPlaying;
        notifyPlayStateChanged();
    }
}

void AudioModel::setVolume(float newVolume)
{
    if (volume.load() != newVolume)
    {
        volume = newVolume;
        notifyVolumeChanged();
    }
}

void AudioModel::setCurrentPosition(double positionInSeconds)
{
    if (currentPositionInSeconds.load() != positionInSeconds)
    {
        currentPositionInSeconds = positionInSeconds;
        notifyPositionChanged();
    }
}

// Private 알림 메서드 구현 - 비동기 호출로 변경

void AudioModel::notifyPlayStateChanged()
{
    // 현재 상태 캡처
    bool isNowPlaying = playing.load();
    
    // 메인 리스너 호출
    safelyCallListeners(legacyListeners, [isNowPlaying](Listener& l) {
        l.playbackStateChanged(isNowPlaying);
    });
    
    // 새 리스너 인터페이스 호출
    safelyCallListeners(listeners, [isNowPlaying](IAudioModelListener& l) {
        l.onPlayStateChanged(isNowPlaying);
    });
}

void AudioModel::notifyInputLevelChanged()
{
    // 현재 값 캡처
    float currentLevel = currentInputLevel.load();
    
    // 비동기 호출: juce::MessageManager::callAsync를 사용하여
    // 메인 스레드에서 리스너 알림이 발생하도록 함
    safelyCallListeners(legacyListeners, [currentLevel](Listener& l) {
        l.inputLevelChanged(currentLevel);
    });
    
    safelyCallListeners(listeners, [currentLevel](IAudioModelListener& l) {
        l.onInputLevelChanged(currentLevel);
    });
}

void AudioModel::notifyVolumeChanged()
{
    // 현재 값 캡처
    float currentVol = volume.load();
    
    safelyCallListeners(legacyListeners, [currentVol](Listener& l) {
        l.volumeChanged(currentVol);
    });
    
    safelyCallListeners(listeners, [currentVol](IAudioModelListener& l) {
        l.onVolumeChanged(currentVol);
    });
}

void AudioModel::notifyPositionChanged()
{
    // 현재 값 캡처
    double position = currentPositionInSeconds.load();
    
    safelyCallListeners(legacyListeners, [position](Listener& l) {
        l.playbackPositionChanged(position);
    });
    
    safelyCallListeners(listeners, [position](IAudioModelListener& l) {
        l.onPositionChanged(position);
    });
}