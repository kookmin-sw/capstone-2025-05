#include "ContentPanelComponent.h"
#include "MainComponent.h"

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
    
    // 모델 데이터 초기화
    contentModel.initializeSampleData();
    
    // 모델 데이터를 뷰에 표시
    try {
        // 연습 가능한 곡 표시
        for (const auto& song : contentModel.getRecentSongs())
        {
            recentGrid.addSong(song, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        }
        
        // 추천 곡 표시
        for (const auto& song : contentModel.getRecommendedSongs())
        {
            recommendGrid.addSong(song, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        }
    }
    catch (const std::exception& e) {
        DBG("Exception in ContentPanelComponent: " + juce::String(e.what()));
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
    auto bounds = getLocalBounds().reduced(10);
    
    // Recent 섹션 레이아웃
    recentTitle.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(5); // 간격
    
    auto recentSection = bounds.removeFromTop(200);
    recentGrid.setBounds(recentSection.withTrimmedRight(100));
    viewAllRecent.setBounds(recentSection.removeFromRight(90).withTrimmedTop(5));
    
    bounds.removeFromTop(20); // 섹션 간 간격
    
    // Recommended 섹션 레이아웃
    recommendTitle.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(5); // 간격
    
    auto recommendSection = bounds.removeFromTop(200);
    recommendGrid.setBounds(recommendSection.withTrimmedRight(100));
    viewAllRecommend.setBounds(recommendSection.removeFromRight(90).withTrimmedTop(5));
}