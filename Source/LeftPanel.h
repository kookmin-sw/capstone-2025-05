#pragma once
#include <JuceHeader.h>

class LeftPanel : public juce::Component {
public:
    LeftPanel();
    ~LeftPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider volumeSlider;
    juce::Component levelMeter; // 커스텀 레벨미터 (나중에 상속으로 구현)
    juce::ComboBox inputSelector; // 입력 소스 선택
    juce::ToggleButton distortionToggle{"Distortion"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeftPanel)
};