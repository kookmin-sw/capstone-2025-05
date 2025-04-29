#include "PracticeSettingsComponent.h"

PracticeSettingsComponent::PracticeSettingsComponent()
{
    // 기본 스타일 설정
    setOpaque(true);
    
    // 제목 레이블
    titleLabel.setText("Setting Practice", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    // 템포 섹션 설정
    tempoLabel.setText("Tempo", juce::dontSendNotification);
    tempoLabel.setFont(juce::Font(16.0f));
    addAndMakeVisible(tempoLabel);
    
    tempoSlider.setRange(40.0, 240.0, 1.0);
    tempoSlider.setValue(tempo, juce::dontSendNotification);
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 24);
    tempoSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    tempoSlider.onValueChange = [this]() {
        tempo = static_cast<int>(tempoSlider.getValue());
        if (onSettingsChanged) onSettingsChanged();
    };
    addAndMakeVisible(tempoSlider);
    
    tapTempoButton.setButtonText("Tap Tempo");
    tapTempoButton.onClick = [this]() {
        // 탭 템포 기능 구현
        // TODO: 구현 필요
    };
    addAndMakeVisible(tapTempoButton);
    
    // 루프 섹션 설정
    loopToggle.setButtonText("Enable Loop");
    loopToggle.setToggleState(loopEnabled, juce::dontSendNotification);
    loopToggle.onClick = [this]() {
        loopEnabled = loopToggle.getToggleState();
        loopStartSlider.setEnabled(loopEnabled);
        loopEndSlider.setEnabled(loopEnabled);
        if (onSettingsChanged) onSettingsChanged();
    };
    addAndMakeVisible(loopToggle);
    
    loopRangeLabel.setText("Loop Range", juce::dontSendNotification);
    loopRangeLabel.setFont(juce::Font(16.0f));
    addAndMakeVisible(loopRangeLabel);
    
    loopStartSlider.setRange(1.0, 100.0, 1.0);
    loopStartSlider.setValue(loopStartMeasure, juce::dontSendNotification);
    loopStartSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 24);
    loopStartSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    loopStartSlider.setEnabled(loopEnabled);
    loopStartSlider.onValueChange = [this]() {
        loopStartMeasure = static_cast<int>(loopStartSlider.getValue());
        loopEndSlider.setRange(loopStartMeasure, 100.0, 1.0);
        if (onSettingsChanged) onSettingsChanged();
    };
    addAndMakeVisible(loopStartSlider);
    
    loopEndSlider.setRange(loopStartMeasure, 100.0, 1.0);
    loopEndSlider.setValue(loopEndMeasure, juce::dontSendNotification);
    loopEndSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 24);
    loopEndSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    loopEndSlider.setEnabled(loopEnabled);
    loopEndSlider.onValueChange = [this]() {
        loopEndMeasure = static_cast<int>(loopEndSlider.getValue());
        if (onSettingsChanged) onSettingsChanged();
    };
    addAndMakeVisible(loopEndSlider);
    
    // 재생 속도 섹션
    speedLabel.setText("Playback Speed", juce::dontSendNotification);
    speedLabel.setFont(juce::Font(16.0f));
    addAndMakeVisible(speedLabel);
    
    speedSlider.setRange(0.25, 2.0, 0.05);
    speedSlider.setValue(playbackSpeed, juce::dontSendNotification);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 24);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
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
    
    // 섹션 구분선 그리기
    g.setColour(MapleTheme::getBackgroundColour());
    auto bounds = getLocalBounds();
    int sectionHeight = bounds.getHeight() / 3;
    
    g.drawLine(20.0f, sectionHeight, bounds.getWidth() - 20.0f, sectionHeight, 1.0f);
    g.drawLine(20.0f, sectionHeight * 2, bounds.getWidth() - 20.0f, sectionHeight * 2, 1.0f);
}

void PracticeSettingsComponent::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    int sectionHeight = bounds.getHeight() / 3;
    
    // 제목 레이블
    titleLabel.setBounds(bounds.getX(), 10, bounds.getWidth(), 30);
    
    // 템포 섹션
    auto tempoSection = bounds.removeFromTop(sectionHeight).reduced(0, 10);
    tempoLabel.setBounds(tempoSection.getX(), tempoSection.getY(), tempoSection.getWidth(), 30);
    
    auto tempoControlArea = tempoSection.removeFromBottom(tempoSection.getHeight() - 30);
    tempoSlider.setBounds(tempoControlArea.getX(), tempoControlArea.getY(), 
                          tempoControlArea.getWidth() - 100, 30);
    
    tapTempoButton.setBounds(tempoSlider.getRight() + 10, tempoControlArea.getY(), 
                            90, 30);
    
    // 루프 섹션
    auto loopSection = bounds.removeFromTop(sectionHeight).reduced(0, 10);
    loopToggle.setBounds(loopSection.getX(), loopSection.getY(), loopSection.getWidth(), 30);
    
    loopRangeLabel.setBounds(loopSection.getX(), loopToggle.getBottom() + 10, 
                            loopSection.getWidth(), 25);
    
    auto loopSlidersArea = loopSection.removeFromBottom(loopSection.getHeight() - 65);
    loopStartSlider.setBounds(loopSlidersArea.getX(), loopSlidersArea.getY(), 
                             loopSlidersArea.getWidth() / 2 - 10, 30);
    
    loopEndSlider.setBounds(loopStartSlider.getRight() + 20, loopSlidersArea.getY(),
                           loopSlidersArea.getWidth() / 2 - 10, 30);
    
    // 재생 속도 섹션
    auto speedSection = bounds.reduced(0, 10);
    speedLabel.setBounds(speedSection.getX(), speedSection.getY(), speedSection.getWidth(), 30);
    
    speedSlider.setBounds(speedSection.getX(), speedLabel.getBottom() + 10, 
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