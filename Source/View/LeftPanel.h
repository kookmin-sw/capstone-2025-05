#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"
#include "Model/IAudioModelListener.h"

class LeftPanel : public juce::Component, 
                 public IAudioModelListener
{
public:
    LeftPanel(AudioModel& model);
    ~LeftPanel() override;
    
    // Component 오버라이드
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // IAudioModelListener 인터페이스 구현
    void onPlayStateChanged(bool isPlaying) override;
    void onVolumeChanged(float newVolume) override;
    void onPositionChanged(double positionInSeconds) override;
    
    // 패널 관련 메서드
    void initialize();
    void updatePanel();
    void resetPanel();
    
private:
    AudioModel& audioModel;
    
    // 입력 레벨 표시를 위한 UI 요소
    double progressValue = 0.0;
    std::unique_ptr<juce::Label> levelLabel;
    std::unique_ptr<juce::ProgressBar> levelMeter;
    
    // 볼륨 컨트롤을 위한 UI 요소
    std::unique_ptr<juce::Slider> volumeSlider;
    std::unique_ptr<juce::Label> volumeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeftPanel)
};