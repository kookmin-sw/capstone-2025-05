#pragma once
#include <JuceHeader.h>

class RightPanel : public juce::Component {
public:
    RightPanel();
    ~RightPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    std::unique_ptr<juce::Slider> bpmSlider; // 메트로놈 BPM 조절
    std::unique_ptr<juce::ToggleButton> metronomeToggle;
    std::unique_ptr<juce::Component> tunerComponent; // 튜너 컴포넌트 (향후 구현 예정)
    std::unique_ptr<juce::TextEditor> notesEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RightPanel)
};