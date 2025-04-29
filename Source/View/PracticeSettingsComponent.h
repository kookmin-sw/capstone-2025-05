#pragma once
#include <JuceHeader.h>
#include "LookAndFeel/MapleTheme.h"

/**
 * 기타 연습용 설정 컴포넌트
 * 연습 모드, 템포, 반복 설정 등을 조절할 수 있는 UI
 */
class PracticeSettingsComponent : public juce::Component
{
public:
    PracticeSettingsComponent();
    ~PracticeSettingsComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // 설정값 변경 시 호출되는 콜백 함수 타입
    using SettingsChangeCallback = std::function<void()>;
    void setOnSettingsChanged(SettingsChangeCallback callback) { onSettingsChanged = callback; }
    
    // 설정값 접근자
    int getTempo() const { return tempo; }
    bool getLoopEnabled() const { return loopEnabled; }
    int getLoopStartMeasure() const { return loopStartMeasure; }
    int getLoopEndMeasure() const { return loopEndMeasure; }
    float getPlaybackSpeed() const { return playbackSpeed; }
    
    // 설정값 설정자
    void setTempo(int newTempo);
    void setLoopEnabled(bool enabled);
    void setLoopRange(int startMeasure, int endMeasure);
    void setPlaybackSpeed(float speed);
    
private:
    // UI 컴포넌트들
    juce::Label titleLabel;
    
    // 템포 섹션
    juce::Label tempoLabel;
    juce::Slider tempoSlider;
    juce::TextButton tapTempoButton;
    
    // 루프 섹션
    juce::ToggleButton loopToggle;
    juce::Label loopRangeLabel;
    juce::Slider loopStartSlider;
    juce::Slider loopEndSlider;
    
    // 재생 속도 섹션
    juce::Label speedLabel;
    juce::Slider speedSlider;
    
    // 설정값 저장
    int tempo = 120;
    bool loopEnabled = false;
    int loopStartMeasure = 1;
    int loopEndMeasure = 4;
    float playbackSpeed = 1.0f;
    
    // 콜백
    SettingsChangeCallback onSettingsChanged;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PracticeSettingsComponent)
}; 