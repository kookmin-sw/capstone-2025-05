#include "RightPanel.h"
#include "LookAndFeel/MapleTheme.h"

RightPanel::RightPanel() {
    // BPM 슬라이더 초기화
    bpmSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, 
                                              juce::Slider::TextEntryBoxPosition::TextBoxBelow);
    bpmSlider->setRange(40.0, 240.0, 1.0);
    bpmSlider->setValue(120.0);
    bpmSlider->setColour(juce::Slider::rotarySliderFillColourId, MapleTheme::getAccentColour());
    bpmSlider->setColour(juce::Slider::textBoxTextColourId, MapleTheme::getTextColour());
    bpmSlider->setColour(juce::Slider::textBoxOutlineColourId, MapleTheme::getBackgroundColour());
    bpmSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(bpmSlider.get());
    
    // 메트로놈 토글 버튼 초기화
    metronomeToggle = std::make_unique<juce::ToggleButton>("Metronome");
    metronomeToggle->setColour(juce::ToggleButton::tickColourId, MapleTheme::getAccentColour());
    metronomeToggle->setColour(juce::ToggleButton::tickDisabledColourId, MapleTheme::getAccentColour().withAlpha(0.5f));
    addAndMakeVisible(metronomeToggle.get());
    
    // 노트 에디터 초기화
    notesEditor = std::make_unique<juce::TextEditor>("Notes");
    notesEditor->setMultiLine(true);
    notesEditor->setReturnKeyStartsNewLine(true);
    notesEditor->setReadOnly(false);
    notesEditor->setScrollbarsShown(true);
    notesEditor->setText("Project Notes");
    notesEditor->setColour(juce::TextEditor::backgroundColourId, MapleTheme::getBackgroundColour().darker(0.2f));
    notesEditor->setColour(juce::TextEditor::textColourId, MapleTheme::getTextColour());
    notesEditor->setColour(juce::TextEditor::outlineColourId, MapleTheme::getCardColour());
    addAndMakeVisible(notesEditor.get());
    
    // 튜너 컴포넌트 초기화 (향후 구현 예정)
    tunerComponent = std::make_unique<juce::Component>();
    tunerComponent->setVisible(true);
    addAndMakeVisible(tunerComponent.get());
}

RightPanel::~RightPanel() {}

void RightPanel::paint(juce::Graphics& g) {
    // 패널 배경
    g.fillAll(MapleTheme::getBackgroundColour());
    g.setColour(MapleTheme::getCardColour());
    g.drawRect(getLocalBounds(), 1);
    
    // 패널 제목
    g.setFont(18.0f);
    g.setColour(MapleTheme::getTextColour());
    g.drawText("Tools", 10, 10, getWidth() - 20, 20, juce::Justification::centredLeft);
}

void RightPanel::resized() {
    auto area = getLocalBounds().reduced(10);
    auto topSection = area.removeFromTop(150);
    
    // 튜너 영역
    tunerComponent->setBounds(topSection.removeFromTop(80));
    
    topSection.removeFromTop(10); // 여백
    
    // 메트로놈 영역
    auto metronomeArea = topSection.removeFromTop(50);
    metronomeToggle->setBounds(metronomeArea.removeFromLeft(100));
    bpmSlider->setBounds(metronomeArea);
    
    area.removeFromTop(10); // 여백
    
    // 노트 영역
    notesEditor->setBounds(area);
}