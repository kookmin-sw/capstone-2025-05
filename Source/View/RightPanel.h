#pragma once
#include <JuceHeader.h>

class RightPanel : public juce::Component {
public:
    RightPanel();
    ~RightPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider bpmSlider; // 메트로놈 BPM 조절
    juce::ToggleButton metronomeToggle{"Metronome"};
    juce::Component tuner; // 튜너 (나중에 상속으로 구현)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RightPanel)
};