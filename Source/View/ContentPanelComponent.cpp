#include "ContentPanelComponent.h"
#include "MainComponent.h"
#include "Controller/ContentController.h"

ContentPanelComponent::ContentPanelComponent()
{
    // UI 컴포넌트 초기화
    addAndMakeVisible(recentTitle);
    recentTitle.setText("Playable Songs", juce::dontSendNotification);
    recentTitle.setFont(juce::Font(18.0f, juce::Font::bold));
    
    addAndMakeVisible(recentGrid);
    
    addAndMakeVisible(viewAllRecent);
    viewAllRecent.setButtonText("View All");
    
    addAndMakeVisible(recommendTitle);
    recommendTitle.setText("Recommended Songs", juce::dontSendNotification);
    recommendTitle.setFont(juce::Font(18.0f, juce::Font::bold));
    
    addAndMakeVisible(recommendGrid);
    
    addAndMakeVisible(viewAllRecommend);
    viewAllRecommend.setButtonText("View All");
    
    // 로딩 인디케이터 초기화
    addAndMakeVisible(loadingLabel);
    loadingLabel.setText("Loading songs...", juce::dontSendNotification);
    loadingLabel.setJustificationType(juce::Justification::centred);
    loadingLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    loadingLabel.setVisible(false);
}

void ContentPanelComponent::setContentController(ContentController* controller)
{
    contentController = controller;
    
    // ContentController를 그리드 뷰에 설정
    recentGrid.setContentController(contentController);
    recommendGrid.setContentController(contentController);
}

void ContentPanelComponent::initialize()
{
    // ContentController가 설정되어 있는지 확인
    if (contentController == nullptr)
    {
        DBG("ContentPanelComponent: ContentController not set");
        return;
    }
    
    // 먼저 로컬 데이터로 초기화 (초기 모습을 보여주기 위함)
    updateGridViews();
    
    // API에서 데이터 로드
    loadDataFromAPI();
}

void ContentPanelComponent::loadDataFromAPI()
{
    if (contentController == nullptr)
        return;
        
    // 로딩 상태 표시
    isLoading = true;
    loadingLabel.setVisible(true);
    
    // API에서 데이터 로드
    contentController->loadSongsFromAPI([this](bool success) {
        isLoading = false;
        loadingLabel.setVisible(false);
        
        if (success)
        {
            // 화면 업데이트
            updateGridViews();
        }
        else
        {
            // 로드 실패 시 오류 메시지 표시
            loadingLabel.setText("Failed to load songs", juce::dontSendNotification);
            loadingLabel.setVisible(true);
            
            // 3초 후 메시지 숨기기
            juce::Timer::callAfterDelay(3000, [this]() {
                loadingLabel.setVisible(false);
            });
        }
    });
}

void ContentPanelComponent::updateGridViews()
{
    if (contentController == nullptr)
        return;
        
    try {
        // 그리드 초기화
        recentGrid.clear();
        recommendGrid.clear();
        
        // 디버깅 로그 추가
        DBG("ContentPanelComponent::updateGridViews - start");
        DBG("Recent songs count: " + juce::String(contentController->getRecentSongs().size()));
        DBG("Recommended songs count: " + juce::String(contentController->getRecommendedSongs().size()));
        
        // 연습 가능한 곡 표시
        for (const auto& song : contentController->getRecentSongs())
        {
            DBG("Adding recent song: " + song.getTitle() + " (ID: " + song.getId() + ")");
            recentGrid.addSong(song, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        }
        
        // 추천 곡 표시
        for (const auto& song : contentController->getRecommendedSongs())
        {
            DBG("Adding recommended song: " + song.getTitle() + " (ID: " + song.getId() + ")");
            recommendGrid.addSong(song, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        }
        
        DBG("ContentPanelComponent::updateGridViews - complete");
    }
    catch (const std::exception& e) {
        DBG("Exception in ContentPanelComponent::updateGridViews: " + juce::String(e.what()));
    }
}

// 곡 선택 이벤트를 리스너에게 통지
void ContentPanelComponent::notifySongSelected(const juce::String& songId)
{
    DBG("Song selected: " + songId);
    // ListenerList를 사용하여 이벤트 통지
    songListeners.call(&SongSelectedListener::songSelected, songId);
}

void ContentPanelComponent::resized()
{
    DBG("ContentPanelComponent::resized - start");
    
    auto bounds = getLocalBounds().reduced(10);
    DBG("ContentPanelComponent::resized - component size: " + juce::String(bounds.getWidth()) + "x" + juce::String(bounds.getHeight()));
    
    // 로딩 인디케이터 위치 설정
    loadingLabel.setBounds(bounds.withSizeKeepingCentre(200, 30));
    
    // Recent 섹션 레이아웃
    recentTitle.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(5); // 간격
    
    auto recentSection = bounds.removeFromTop(200);
    recentGrid.setBounds(recentSection.withTrimmedRight(100));
    viewAllRecent.setBounds(recentSection.removeFromRight(90).withTrimmedTop(5));
    
    DBG("ContentPanelComponent::resized - recent grid position: " + 
        juce::String(recentGrid.getX()) + "," + juce::String(recentGrid.getY()) + 
        " size: " + juce::String(recentGrid.getWidth()) + "x" + juce::String(recentGrid.getHeight()));
    
    bounds.removeFromTop(20); // 섹션 간 간격
    
    // Recommended 섹션 레이아웃
    recommendTitle.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(5); // 간격
    
    auto recommendSection = bounds.removeFromTop(200);
    recommendGrid.setBounds(recommendSection.withTrimmedRight(100));
    viewAllRecommend.setBounds(recommendSection.removeFromRight(90).withTrimmedTop(5));
    
    DBG("ContentPanelComponent::resized - recommended grid position: " + 
        juce::String(recommendGrid.getX()) + "," + juce::String(recommendGrid.getY()) + 
        " size: " + juce::String(recommendGrid.getWidth()) + "x" + juce::String(recommendGrid.getHeight()));
    
    DBG("ContentPanelComponent::resized - complete");
}