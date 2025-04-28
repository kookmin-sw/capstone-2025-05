#pragma once
#include <JuceHeader.h>
#include "Components/SidebarComponent.h"
#include "Components/SearchHeaderComponent.h"
#include "Components/SongGridView.h"
#include "LookAndFeel/MapleTheme.h"

// 전방 선언
class GuitarPracticeComponent;
class ContentController;

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
    
    // 곡 선택 처리
    void handleSongSelected(const juce::String& songId);
    
    // 테스트용 더미 데이터 추가
    void addDummySongs();
    
    // UI 테마
    MapleTheme theme;
    
    // 오디오 시스템
    juce::AudioDeviceManager deviceManager;
    
    // UI 컴포넌트 (Spotify 스타일)
    std::unique_ptr<SidebarComponent> sidebarComponent;
    std::unique_ptr<SearchHeaderComponent> headerComponent;
    std::unique_ptr<SongGridView> featuredSongsView;
    std::unique_ptr<SongGridView> recentlyPlayedView;
    std::unique_ptr<SongGridView> recommendedView;
    
    // 연습 화면
    std::unique_ptr<GuitarPracticeComponent> guitarPracticeComponent;
    
    // 컨트롤러
    std::shared_ptr<ContentController> contentController;
    
    // 스크롤 뷰 (메인 콘텐츠용)
    juce::Viewport mainContentViewport;
    juce::Component mainContentContainer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};