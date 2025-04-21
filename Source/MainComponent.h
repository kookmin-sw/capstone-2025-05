#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"

// 전방 선언
class HeaderComponent;
class MainActionComponent;
class BottomComponent;
class GuitarPracticeComponent;
class ContentPanelComponent;
class ContentController;

// MainComponent는 View 역할만 담당하도록 수정
class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // 화면 전환 메서드 (View 관점에서)
    void showMainScreen();
    void showPracticeScreen();
    
    // 디바이스 매니저 접근자 (나중에 AudioController로 완전히 이동 예정)
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }
    
    // 앱 종료 전 정리 메서드 추가
    void prepareToClose();

private:
    // 오디오 관련 - 나중에 AudioController로 이동 예정
    juce::AudioDeviceManager deviceManager;
    
    // UI 컴포넌트들
    std::unique_ptr<HeaderComponent> headerComponent;
    std::unique_ptr<MainActionComponent> mainActionComponent;
    std::unique_ptr<ContentPanelComponent> contentPanelComponent;
    std::unique_ptr<BottomComponent> bottomComponent;
    std::unique_ptr<GuitarPracticeComponent> guitarPracticeComponent;
    
    // Controller 참조
    std::shared_ptr<ContentController> contentController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};