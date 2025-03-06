#include "LeftPanel.h"

// 생성자
LeftPanel::LeftPanel() {
    // 볼륨 슬라이더 설정
    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.setRange(0.0, 1.0, 0.01); // 0~1, 0.01 단위
    volumeSlider.setValue(0.5);           // 초기값 50%
    addAndMakeVisible(volumeSlider);

    // 입력 소스 선택 설정
    inputSelector.addItem("Microphone", 1);
    inputSelector.addItem("Audio Interface", 2);
    inputSelector.addItem("File Input", 3);
    inputSelector.setSelectedId(1); // 기본값: 마이크
    addAndMakeVisible(inputSelector);

    // 디스토션 토글 설정
    distortionToggle.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(distortionToggle);

    // 레벨미터는 일단 빈 껍데기
    addAndMakeVisible(levelMeter);
}

// 소멸자
LeftPanel::~LeftPanel() {
    // 지금은 추가 정리 필요 없음
}

// 화면 그리기
void LeftPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey); // 배경 어두운 회색
}

// 레이아웃 배치
void LeftPanel::resized() {
    auto bounds = getLocalBounds().reduced(10); // 10px 여백

    // 세로로 쌓기 (위에서 아래로)
    volumeSlider.setBounds(bounds.removeFromTop(150));    // 높이 150px
    levelMeter.setBounds(bounds.removeFromTop(100));      // 높이 100px
    inputSelector.setBounds(bounds.removeFromTop(30));    // 높이 30px
    distortionToggle.setBounds(bounds.removeFromTop(30)); // 높이 30px
}