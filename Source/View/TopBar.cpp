#include "TopBar.h"
#include "GuitarPracticeComponent.h" // Now include the full header here
#include "LookAndFeel/MapleTheme.h"
#include "Dialog/AudioSettingsDialog.h" // 추가: 오디오 설정 다이얼로그 헤더

TopBar::TopBar(GuitarPracticeComponent &parent)
    : parentComponent(parent)
{
    // 오디오 설정 버튼 초기화
    audioSettingsButton.setButtonText(juce::String::fromUTF8("🔊 오디오 설정"));
    audioSettingsButton.onClick = [this]() {
        // 부모 컴포넌트에서 deviceManager 접근
        juce::AudioDeviceManager& deviceManager = parentComponent.getDeviceManager();
        // 오디오 설정 다이얼로그 표시
        AudioSettingsDialog::show(deviceManager);
    };
    addAndMakeVisible(audioSettingsButton);
}

TopBar::~TopBar() {}

void TopBar::paint(juce::Graphics &g)
{
    g.fillAll(MapleTheme::getCardColour());
    
    // 제목 표시
    g.setColour(MapleTheme::getTextColour());
    g.setFont(18.0f);
    g.drawText("Guitar Practice Mode", getLocalBounds(), juce::Justification::centred, true);
}

void TopBar::resized()
{
    // 오디오 설정 버튼 위치 및 크기 설정
    const int buttonWidth = 120;
    const int buttonHeight = 30;
    const int margin = 10;
    
    // 우측 상단에 위치
    audioSettingsButton.setBounds(
        getWidth() - buttonWidth - margin,
        margin,
        buttonWidth,
        buttonHeight
    );
}