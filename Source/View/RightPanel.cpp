#include "RightPanel.h"

RightPanel::RightPanel() {
    // 패널 초기화
    initialize();
}

RightPanel::~RightPanel() {}

void RightPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey); // 배경 어두운 회색
    
    // 패널 제목 그리기
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Tools", getLocalBounds().reduced(10).removeFromTop(30),
               juce::Justification::centred, true);
}

void RightPanel::resized() {
    auto bounds = getLocalBounds().reduced(10);
    
    // 패널 제목을 위한 공간
    bounds.removeFromTop(40);
    
    // 메트로놈 컨트롤
    metronomeToggle.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(10); // 간격
    bpmSlider.setBounds(bounds.removeFromTop(40));
    
    bounds.removeFromTop(20); // 간격
    
    // 노트 에디터
    if (notesEditor != nullptr)
        notesEditor->setBounds(bounds.removeFromTop(200));
    
    bounds.removeFromTop(20); // 간격
    
    // 튜너 영역 (나중에 구현)
    tuner.setBounds(bounds.removeFromTop(100));
}

// IPanelComponent 인터페이스 구현
void RightPanel::initialize() {
    // 메트로놈 관련 UI 설정
    addAndMakeVisible(metronomeToggle);
    
    addAndMakeVisible(bpmSlider);
    bpmSlider.setRange(40.0, 220.0, 1.0);
    bpmSlider.setValue(120.0);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    
    // 노트 에디터 설정
    notesEditor = std::make_unique<juce::TextEditor>("notesEditor");
    notesEditor->setMultiLine(true);
    notesEditor->setReturnKeyStartsNewLine(true);
    notesEditor->setReadOnly(false);
    notesEditor->setScrollbarsShown(true);
    notesEditor->setCaretVisible(true);
    notesEditor->setPopupMenuEnabled(true);
    notesEditor->setText("Enter your practice notes...");
    addAndMakeVisible(notesEditor.get());
    
    // 튜너 UI는 나중에 구현
    addAndMakeVisible(tuner);
}

void RightPanel::updatePanel() {
    // 패널 UI 업데이트
    repaint();
}

void RightPanel::resetPanel() {
    // 패널 상태 초기화
    bpmSlider.setValue(120.0, juce::dontSendNotification);
    metronomeToggle.setToggleState(false, juce::dontSendNotification);
    
    if (notesEditor != nullptr)
        notesEditor->setText("Enter your practice notes...");
    
    repaint();
}