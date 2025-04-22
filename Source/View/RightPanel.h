#pragma once
#include <JuceHeader.h>
#include "IPanelComponent.h"

class RightPanel : public juce::Component, public IPanelComponent {
public:
    RightPanel();
    ~RightPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // IPanelComponent 인터페이스 구현
    void initialize() override;
    void updatePanel() override;
    void resetPanel() override;
    juce::Component* asComponent() override { return this; }

private:
    juce::Slider bpmSlider; // 메트로놈 BPM 조절
    juce::ToggleButton metronomeToggle{"Metronome"};
    juce::Component tuner; // 튜너 (나중에 상속으로 구현)
    std::unique_ptr<juce::TextEditor> notesEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RightPanel)
};