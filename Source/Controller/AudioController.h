#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"
#include "Model/AudioTap.h"
#include "AmpliTubeProcessor.h"

// 전방 선언
class GuitarPracticeController;

class AudioController : public juce::AudioIODeviceCallback {
public:
    AudioController(AudioModel& model, juce::AudioDeviceManager& deviceManager);
    ~AudioController() override;
    
    // GuitarPracticeController 설정 메서드 추가
    void setGuitarPracticeController(GuitarPracticeController* controller);
    
    // 마이크 모니터링 기능 추가
    void enableMicrophoneMonitoring(bool shouldEnable);
    bool isMicrophoneMonitoringEnabled() const { return microphoneMonitoringEnabled; }
    
    // 게인 설정 메서드 추가 
    void setMicrophoneGain(float gain);
    float getMicrophoneGain() const { return microphoneGain; }
    
    // AudioIODeviceCallback implementation
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, 
                                         int numInputChannels,
                                         float* const* outputChannelData, 
                                         int numOutputChannels,
                                         int numSamples,
                                         const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    
    // AmpliTube 플러그인 관련 메서드
    void enableAmpliTubeEffect(bool shouldEnable);
    bool isAmpliTubeEffectEnabled() const;
    juce::Component* getAmpliTubeEditorComponent();
    
    // AmpliTube 게인 제어 메서드 추가
    void setAmpliTubeInputGain(float gain);
    float getAmpliTubeInputGain() const;

    // 오디오 탭 접근자 - 시각화에 필요한 데이터 제공
    AudioTap& getAudioTap() { return audioTap; }
    
private:
    AudioModel& audioModel;
    juce::AudioDeviceManager& deviceManager;
    
    // GuitarPracticeController 포인터 추가 (약한 참조 - 소유권 없음)
    GuitarPracticeController* guitarPracticeController;
    
    // 마이크 모니터링을 위한 변수
    bool microphoneMonitoringEnabled = false;
    float microphoneGain = 5.0f; // 마이크 모니터링 볼륨
    
    // AmpliTube 이펙터 처리기
    std::unique_ptr<AmpliTubeProcessor> ampliTubeProcessor;

    // 오디오 시각화를 위한 락-프리 탭
    AudioTap audioTap;
    
    // 입력 레벨 계산용 임시 버퍼 (초기화 시 한 번만 할당)
    juce::AudioBuffer<float> levelMeterBuffer;
};