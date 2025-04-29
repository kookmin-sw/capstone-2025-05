#pragma once
#include <JuceHeader.h>
#include "Components/SidebarComponent.h"
#include "Components/SearchHeaderComponent.h"
#include "ContentPanelComponent.h"
#include "LookAndFeel/MapleTheme.h"

// 전방 선언
class GuitarPracticeComponent;
class ContentController;
class ContentModel;

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // 화면 전환 메서드
    void showMainScreen();
    void showPracticeScreen();
    
    // 오디오 디바이스 매니저 접근자
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }
    
    // 애플리케이션 종료 전 필요한 정리 작업 수행
    void prepareToClose();

private:
    // 메뉴 선택 처리
    void handleMenuItemSelected(SidebarComponent::ItemType menuItem);
    
    // UI 테마
    MapleTheme theme;
    
    // 오디오 시스템
    juce::AudioDeviceManager deviceManager;
    
    // UI 컴포넌트
    std::unique_ptr<SidebarComponent> sidebarComponent;
    std::unique_ptr<SearchHeaderComponent> headerComponent;
    
    // 콘텐츠 패널 (API에서 데이터 로드 및 표시)
    std::unique_ptr<ContentPanelComponent> contentPanelComponent;
    
    // 연습 화면
    std::unique_ptr<GuitarPracticeComponent> guitarPracticeComponent;
    
    // MVC 구성 요소
    std::shared_ptr<ContentModel> contentModel;
    std::shared_ptr<ContentController> contentController;
    
    // 스크롤 뷰 (메인 콘텐츠용)
    juce::Viewport mainContentViewport;
    juce::Component mainContentContainer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};