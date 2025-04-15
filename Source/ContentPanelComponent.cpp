#include "ContentPanelComponent.h"

ContentPanelComponent::ContentPanelComponent()
{
    addAndMakeVisible(recentTitle);
    recentTitle.setText("Recent Albums", juce::dontSendNotification);
    recentTitle.setFont(juce::Font(18.0f, juce::Font::bold));
    
    addAndMakeVisible(recentGrid);
    
    addAndMakeVisible(viewAllRecent);
    viewAllRecent.setButtonText("View All");
    
    addAndMakeVisible(recommendTitle);
    recommendTitle.setText("Recommended Albums", juce::dontSendNotification);
    recommendTitle.setFont(juce::Font(18.0f, juce::Font::bold));
    
    addAndMakeVisible(recommendGrid);
    
    addAndMakeVisible(viewAllRecommend);
    viewAllRecommend.setButtonText("View All");
    
    // 데이터 초기화
    initializeSampleData();
}

void ContentPanelComponent::initializeSampleData()
{
    DBG("Initializing sample data with searchAlbumsAsync...");

    // Recent Albums 검색
    recentTitle.setText("Recent Albums (Loading...)", juce::dontSendNotification);
    SpotifyService::searchAlbumsAsync("Younha", [this](juce::Array<SpotifyService::Album> albums) {
        DBG("Recent albums search results: " + juce::String(albums.size()) + " albums found");
        if (albums.isEmpty())
        {
            DBG("No recent albums returned from search");
            recentTitle.setText("Recent Albums (No Data)", juce::dontSendNotification);
        }
        else
        {
            recentTitle.setText("Recent Albums", juce::dontSendNotification);
            int count = std::min(4, albums.size()); // 최대 4개만 표시
            for (int i = 0; i < count; i++)
            {
                recentGrid.addAlbumFromSpotify(albums[i]);
                DBG("Added to recentGrid: " + albums[i].name);
            }
        }
        repaint();
    });

    // Recommended Albums 검색
    recommendTitle.setText("Recommended Albums (Loading...)", juce::dontSendNotification);
    SpotifyService::searchAlbumsAsync("IU", [this](juce::Array<SpotifyService::Album> albums) {
        DBG("Recommended albums search results: " + juce::String(albums.size()) + " albums found");
        if (albums.isEmpty())
        {
            DBG("No recommended albums returned from search");
            recommendTitle.setText("Recommended Albums (No Data)", juce::dontSendNotification);
        }
        else
        {
            recommendTitle.setText("Recommended Albums", juce::dontSendNotification);
            for (auto& album : albums)
            {
                recommendGrid.addAlbumFromSpotify(album);
                DBG("Added to recommendGrid: " + album.name);
            }
        }
        repaint();
    });
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