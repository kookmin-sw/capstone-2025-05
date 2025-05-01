#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"

// 전방 선언
class GuitarPracticeController;

class AudioController : public juce::AudioIODeviceCallback {
public:
    AudioController(AudioModel& model, juce::AudioDeviceManager& deviceManager);
    ~AudioController() override;
    
    // GuitarPracticeController 설정 메서드 추가
    void setGuitarPracticeController(GuitarPracticeController* controller);
    
    // AudioIODeviceCallback implementation
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, 
                                         int numInputChannels,
                                         float* const* outputChannelData, 
                                         int numOutputChannels,
                                         int numSamples,
                                         const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    
private:
    AudioModel& audioModel;
    juce::AudioDeviceManager& deviceManager;
    
    // GuitarPracticeController 포인터 추가 (약한 참조 - 소유권 없음)
    GuitarPracticeController* guitarPracticeController;
};