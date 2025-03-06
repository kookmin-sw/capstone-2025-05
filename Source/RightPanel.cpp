#include "RightPanel.h"

RightPanel::RightPanel() {
    // BPM 슬라이더 설정
    bpmSlider.setSliderStyle(juce::Slider::LinearVertical);
    bpmSlider.setRange(40.0, 200.0, 1.0); // 40~200 BPM
    bpmSlider.setValue(120.0);            // 기본값 120 BPM
    addAndMakeVisible(bpmSlider);

    // 메트로놈 토글
    metronomeToggle.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(metronomeToggle);

    // 튜너 (빈 껍데기)
    addAndMakeVisible(tuner);
}

RightPanel::~RightPanel() {}

void RightPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey); // 배경 어두운 회색
}

void RightPanel::resized() {
    auto bounds = getLocalBounds().reduced(10); // 10px 여백
    bpmSlider.setBounds(bounds.removeFromTop(150));      // 높이 150px
    tuner.setBounds(bounds.removeFromTop(100));          // 높이 100px
    metronomeToggle.setBounds(bounds.removeFromTop(30)); // 높이 30px
}