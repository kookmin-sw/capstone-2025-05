#include "PracticeSettingsComponent.h"

PracticeSettingsComponent::PracticeSettingsComponent()
{
    // 기본 스타일 설정
    setOpaque(true);
    
    // 제목 레이블
    titleLabel.setText("Setting Practice", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, MapleTheme::getHighlightColour());
    addAndMakeVisible(titleLabel);
    
    // 템포 섹션 설정
    tempoLabel.setText("Tempo", juce::dontSendNotification);
    tempoLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    tempoLabel.setColour(juce::Label::textColourId, MapleTheme::getTextColour());
    addAndMakeVisible(tempoLabel);
    
    tempoSlider.setRange(40.0, 240.0, 1.0);
    tempoSlider.setValue(tempo, juce::dontSendNotification);
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 24);
    tempoSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    tempoSlider.setColour(juce::Slider::thumbColourId, MapleTheme::getAccentColour());
    tempoSlider.setColour(juce::Slider::trackColourId, MapleTheme::getAccentColour().withAlpha(0.6f));
    tempoSlider.setColour(juce::Slider::textBoxTextColourId, MapleTheme::getTextColour());
    tempoSlider.setColour(juce::Slider::textBoxOutlineColourId, MapleTheme::getBackgroundColour());
    tempoSlider.setColour(juce::Slider::textBoxBackgroundColourId, MapleTheme::getBackgroundColour().brighter(0.1f));
    tempoSlider.onValueChange = [this]() {
        tempo = static_cast<int>(tempoSlider.getValue());
        if (onSettingsChanged) onSettingsChanged();
    };
    addAndMakeVisible(tempoSlider);
    
    tapTempoButton.setButtonText("Tap");
    tapTempoButton.setColour(juce::TextButton::buttonColourId, MapleTheme::getAccentColour());
    tapTempoButton.setColour(juce::TextButton::buttonOnColourId, MapleTheme::getHighlightColour());
    tapTempoButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    tapTempoButton.onClick = [this]() {
        // 탭 템포 기능 구현
        // TODO: 구현 필요
    };
    addAndMakeVisible(tapTempoButton);
    
    // 루프 섹션 설정
    loopToggle.setButtonText("Enable Loop");
    loopToggle.setToggleState(loopEnabled, juce::dontSendNotification);
    loopToggle.setColour(juce::ToggleButton::textColourId, MapleTheme::getTextColour());
    loopToggle.setColour(juce::ToggleButton::tickColourId, MapleTheme::getAccentColour());
    loopToggle.setColour(juce::ToggleButton::tickDisabledColourId, MapleTheme::getSubTextColour());
    loopToggle.onClick = [this]() {
        loopEnabled = loopToggle.getToggleState();
        loopStartSlider.setEnabled(loopEnabled);
        loopEndSlider.setEnabled(loopEnabled);
        if (onSettingsChanged) onSettingsChanged();
        repaint(); // 상태가 변경되면 UI 다시 그리기
    };
    addAndMakeVisible(loopToggle);
    
    loopRangeLabel.setText("Loop Range", juce::dontSendNotification);
    loopRangeLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    loopRangeLabel.setColour(juce::Label::textColourId, MapleTheme::getTextColour());
    addAndMakeVisible(loopRangeLabel);
    
    auto setupLoopSlider = [this](juce::Slider& slider, int defaultValue) {
        slider.setRange(1.0, 100.0, 1.0);
        slider.setValue(defaultValue, juce::dontSendNotification);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 24);
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setEnabled(loopEnabled);
        slider.setColour(juce::Slider::thumbColourId, MapleTheme::getAccentColour());
        slider.setColour(juce::Slider::trackColourId, MapleTheme::getAccentColour().withAlpha(0.6f));
        slider.setColour(juce::Slider::textBoxTextColourId, MapleTheme::getTextColour());
        slider.setColour(juce::Slider::textBoxOutlineColourId, MapleTheme::getBackgroundColour());
        slider.setColour(juce::Slider::textBoxBackgroundColourId, MapleTheme::getBackgroundColour().brighter(0.1f));
        addAndMakeVisible(slider);
    };
    
    setupLoopSlider(loopStartSlider, loopStartMeasure);
    loopStartSlider.onValueChange = [this]() {
        loopStartMeasure = static_cast<int>(loopStartSlider.getValue());
        loopEndSlider.setRange(loopStartMeasure, 100.0, 1.0);
        if (onSettingsChanged) onSettingsChanged();
    };
    
    setupLoopSlider(loopEndSlider, loopEndMeasure);
    loopEndSlider.onValueChange = [this]() {
        loopEndMeasure = static_cast<int>(loopEndSlider.getValue());
        if (onSettingsChanged) onSettingsChanged();
    };
    
    // 재생 속도 섹션
    speedLabel.setText("Playback Speed", juce::dontSendNotification);
    speedLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    speedLabel.setColour(juce::Label::textColourId, MapleTheme::getTextColour());
    addAndMakeVisible(speedLabel);
    
    speedSlider.setRange(0.25, 2.0, 0.05);
    speedSlider.setValue(playbackSpeed, juce::dontSendNotification);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 24);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setColour(juce::Slider::thumbColourId, MapleTheme::getAccentColour());
    speedSlider.setColour(juce::Slider::trackColourId, MapleTheme::getAccentColour().withAlpha(0.6f));
    speedSlider.setColour(juce::Slider::textBoxTextColourId, MapleTheme::getTextColour());
    speedSlider.setColour(juce::Slider::textBoxOutlineColourId, MapleTheme::getBackgroundColour());
    speedSlider.setColour(juce::Slider::textBoxBackgroundColourId, MapleTheme::getBackgroundColour().brighter(0.1f));
    speedSlider.onValueChange = [this]() {
        playbackSpeed = static_cast<float>(speedSlider.getValue());
        if (onSettingsChanged) onSettingsChanged();
    };
    speedSlider.setTextValueSuffix("x");
    addAndMakeVisible(speedSlider);
}

PracticeSettingsComponent::~PracticeSettingsComponent()
{
}

void PracticeSettingsComponent::paint(juce::Graphics& g)
{
    // 배경 그리기
    g.fillAll(MapleTheme::getCardColour());
    
    // 테두리 그리기
    g.setColour(MapleTheme::getAccentColour().withAlpha(0.3f));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 8.0f, 1.0f);
    
    // 섹션 구분선 그리기
    auto bounds = getLocalBounds();
    int sectionHeight = bounds.getHeight() / 3;
    
    // 섹션 배경 그리기
    g.setColour(MapleTheme::getBackgroundColour().brighter(0.05f));
    g.fillRoundedRectangle(bounds.removeFromTop(sectionHeight).reduced(10).toFloat(), 6.0f);
    
    auto loopSectionBounds = bounds.removeFromTop(sectionHeight);
    g.fillRoundedRectangle(loopSectionBounds.reduced(10).toFloat(), 6.0f);
    
    g.fillRoundedRectangle(bounds.reduced(10).toFloat(), 6.0f);
    
    // 슬라이더 레이블 추가
    if (loopEnabled) {
        // 루프 슬라이더 위의 레이블
        g.setColour(MapleTheme::getSubTextColour());
        g.setFont(juce::Font(12.0f));
        
        // 슬라이더 위치 기반으로 레이블 위치 계산
        auto loopStartBounds = loopStartSlider.getBounds();
        auto loopEndBounds = loopEndSlider.getBounds();
        
        g.drawText("Start", loopStartBounds.getX(), loopStartBounds.getY() - 18, 
                   loopStartBounds.getWidth(), 18, juce::Justification::centredLeft, true);
        
        g.drawText("End", loopEndBounds.getX(), loopEndBounds.getY() - 18, 
                   loopEndBounds.getWidth(), 18, juce::Justification::centredLeft, true);
    }
    
    // 비활성화된 슬라이더 경고 표시
    if (!loopEnabled) {
        g.setColour(MapleTheme::getSubTextColour().withAlpha(0.3f));
        g.drawText("Loop disabled", loopSectionBounds.removeFromBottom(30), juce::Justification::centredRight, true);
    }
}

void PracticeSettingsComponent::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    int sectionHeight = bounds.getHeight() / 3;
    
    // 제목 레이블
    titleLabel.setBounds(bounds.getX(), 10, bounds.getWidth(), 30);
    
    // 템포 섹션
    auto tempoSection = bounds.removeFromTop(sectionHeight).reduced(10);
    tempoLabel.setBounds(tempoSection.getX(), tempoSection.getY(), tempoSection.getWidth(), 25);
    
    auto tempoControlArea = tempoSection.removeFromBottom(tempoSection.getHeight() - 25).reduced(0, 5);
    tempoSlider.setBounds(tempoControlArea.getX(), tempoControlArea.getY(), 
                          tempoControlArea.getWidth() - 60, 30);
    
    tapTempoButton.setBounds(tempoSlider.getRight() + 10, tempoControlArea.getY(), 
                            50, 30);
    
    // 루프 섹션
    auto loopSection = bounds.removeFromTop(sectionHeight).reduced(10);
    loopToggle.setBounds(loopSection.getX(), loopSection.getY(), loopSection.getWidth(), 25);
    
    loopRangeLabel.setBounds(loopSection.getX(), loopToggle.getBottom() + 5, 
                            loopSection.getWidth(), 25);
    
    auto loopSlidersArea = loopSection.removeFromBottom(loopSection.getHeight() - 55).reduced(0, 5);
    
    // 슬라이더의 레이블은 paint() 메서드에서 그리도록 변경
    // 여기서는 슬라이더 위치만 설정
    loopStartSlider.setBounds(loopSlidersArea.getX(), loopSlidersArea.getY(), 
                             loopSlidersArea.getWidth() / 2 - 10, 30);
    
    loopEndSlider.setBounds(loopStartSlider.getRight() + 20, loopSlidersArea.getY(),
                           loopSlidersArea.getWidth() / 2 - 10, 30);
    
    // 재생 속도 섹션
    auto speedSection = bounds.reduced(10);
    speedLabel.setBounds(speedSection.getX(), speedSection.getY(), speedSection.getWidth(), 25);
    
    speedSlider.setBounds(speedSection.getX(), speedLabel.getBottom() + 5, 
                         speedSection.getWidth(), 30);
}

void PracticeSettingsComponent::setTempo(int newTempo)
{
    tempo = newTempo;
    tempoSlider.setValue(tempo, juce::dontSendNotification);
}

void PracticeSettingsComponent::setLoopEnabled(bool enabled)
{
    loopEnabled = enabled;
    loopToggle.setToggleState(loopEnabled, juce::dontSendNotification);
    loopStartSlider.setEnabled(loopEnabled);
    loopEndSlider.setEnabled(loopEnabled);
    repaint(); // UI 갱신
}

void PracticeSettingsComponent::setLoopRange(int startMeasure, int endMeasure)
{
    loopStartMeasure = startMeasure;
    loopEndMeasure = endMeasure;
    
    loopStartSlider.setValue(loopStartMeasure, juce::dontSendNotification);
    loopEndSlider.setValue(loopEndMeasure, juce::dontSendNotification);
}

void PracticeSettingsComponent::setPlaybackSpeed(float speed)
{
    playbackSpeed = speed;
    speedSlider.setValue(playbackSpeed, juce::dontSendNotification);
} 