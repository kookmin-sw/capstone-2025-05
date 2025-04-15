#include "ContentPanelComponent.h"
#include "MainComponent.h"

ContentPanelComponent::ContentPanelComponent(MainComponent& mainComp)
{
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
    
    // 샘플 악보 데이터 초기화
    initializeSampleData();
}

void ContentPanelComponent::initializeSampleData()
{
    try {
        // 연습 가능한 곡 섹션
        recentGrid.clear();
        recommendGrid.clear();
        
        // 샘플 악보 데이터 추가 (로컬 파일 경로에 의존하지 않도록 수정)
        // 썸네일 파일이 없을 경우를 대비하여 기본 이미지 사용
        juce::File thumbnailDir("D:/audio_dataset/thumbnails");
        if (!thumbnailDir.exists()) {
            DBG("Thumbnail directory does not exist: " + thumbnailDir.getFullPathName());
            // 실제 파일 없이도 thumbnailPath는 유지 (나중에 사용할 수 있음)
        }
        
        // 샘플 악보 데이터 추가
        Song song1("song1", "Homecoming", "Green Day", "D:/audio_dataset/thumbnails/song1.jpg");
        Song song2("song2", "Sweet Child O' Mine", "Guns N' Roses", "D:/audio_dataset/thumbnails/song2.jpg");
        Song song3("song3", "Wonderwall", "Oasis", "D:/audio_dataset/thumbnails/song3.jpg");
        
        // 연습 가능한 곡 썸네일 추가 (에러 처리 추가)
        try {
            recentGrid.addSong(song1, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        } catch (const std::exception& e) {
            DBG("Error adding song1: " + juce::String(e.what()));
        }
        
        try {
            recentGrid.addSong(song2, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        } catch (const std::exception& e) {
            DBG("Error adding song2: " + juce::String(e.what()));
        }
        
        try {
            recentGrid.addSong(song3, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        } catch (const std::exception& e) {
            DBG("Error adding song3: " + juce::String(e.what()));
        }
        
        // 추천 곡 섹션
        Song song4("song4", "Nothing Else Matters", "Metallica", "D:/audio_dataset/thumbnails/song4.jpg");
        Song song5("song5", "Hotel California", "Eagles", "D:/audio_dataset/thumbnails/song5.jpg");
        
        try {
            recommendGrid.addSong(song4, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        } catch (const std::exception& e) {
            DBG("Error adding song4: " + juce::String(e.what()));
        }
        
        try {
            recommendGrid.addSong(song5, [this](const juce::String& songId) {
                notifySongSelected(songId);
            });
        } catch (const std::exception& e) {
            DBG("Error adding song5: " + juce::String(e.what()));
        }
    } catch (const std::exception& e) {
        DBG("Exception in initializeSampleData: " + juce::String(e.what()));
    } catch (...) {
        DBG("Unknown exception in initializeSampleData");
    }
}

// 곡 선택 이벤트를 리스너에게 통지
void ContentPanelComponent::notifySongSelected(const juce::String& songId)
{
    DBG("Song selected: " + songId);
    // ListenerList를 사용하여 이벤트 통지
    songListeners.call(&SongSelectedListener::songSelected, songId);
}

void ContentPanelComponent::loadAlbumsFromCache()
{
    // 이 함수는 더 이상 사용되지 않으므로 비워두거나 제거 가능
    DBG("loadAlbumsFromCache is not used in this implementation");
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