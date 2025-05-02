#include "TopBar.h"
#include "GuitarPracticeComponent.h" // Now include the full header here
#include "View/LookAndFeel/MapleTheme.h"
#include "View/Dialog/AudioSettingsDialog.h" // 경로 수정
#include "MainComponent.h" // 추가: MainComponent 헤더

TopBar::TopBar(GuitarPracticeComponent &parent)
    : parentComponent(parent)
{
    // 뒤로가기 버튼 초기화
    backButton.setButtonText(juce::String::fromUTF8("← 뒤로가기"));
    backButton.onClick = [this]() {
        // 재생 중인 음원 종료
        if (parentComponent.getController() && parentComponent.getController()->isPlaying()) {
            parentComponent.getController()->stopPlayback();
        }
        
        // MainComponent의 showMainScreen 메서드를 호출해 메인 화면으로 돌아감
        MainComponent* mainComp = dynamic_cast<MainComponent*>(parentComponent.getParentComponent());
        if (mainComp != nullptr) {
            mainComp->showMainScreen();
        }
    };
    backButton.setColour(juce::TextButton::buttonColourId, MapleTheme::getAccentColour());
    backButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(backButton);
    
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
    // 버튼 크기 및 여백 설정
    const int buttonWidth = 120;
    const int buttonHeight = 30;
    const int margin = 10;
    
    // 뒤로가기 버튼 - 좌측 상단에 위치
    backButton.setBounds(
        margin,
        margin,
        buttonWidth,
        buttonHeight
    );
    
    // 오디오 설정 버튼 - 우측 상단에 위치
    audioSettingsButton.setBounds(
        getWidth() - buttonWidth - margin,
        margin,
        buttonWidth,
        buttonHeight
    );
}