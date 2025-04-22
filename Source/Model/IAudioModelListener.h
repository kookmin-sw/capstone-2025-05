#pragma once

/**
 * AudioModel의 이벤트를 수신하기 위한 인터페이스입니다.
 * Observer 패턴을 사용하여 Model과 View 사이의 결합도를 낮춥니다.
 */
class IAudioModelListener
{
public:
    virtual ~IAudioModelListener() = default;
    
    /**
     * 재생 상태가 변경되었을 때 호출됩니다.
     * @param isPlaying 현재 재생 중인지 여부
     */
    virtual void onPlayStateChanged(bool isPlaying) = 0;
    
    /**
     * 볼륨이 변경되었을 때 호출됩니다.
     * @param newVolume 새 볼륨 값 (0.0 ~ 1.0)
     */
    virtual void onVolumeChanged(float newVolume) = 0;
    
    /**
     * 재생 위치가 변경되었을 때 호출됩니다.
     * @param positionInSeconds 현재 재생 위치 (초 단위)
     */
    virtual void onPositionChanged(double positionInSeconds) = 0;
};