#pragma once

#include <JuceHeader.h>
#include "Screen.h"
#include "SidebarButton.h"
#include "FriendActivityItem.h"
#include "SongThumbnail.h"

class HomeComponent : public juce::Component
{
public:
    HomeComponent(std::function<void(Screen)> screenChangeCallback);
    ~HomeComponent() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void drawNavigationMenu(juce::Graphics &g);
    void drawMainContent(juce::Graphics &g);
    void drawRealTimeAnalysis(juce::Graphics &g, juce::Rectangle<int> bounds);
    void drawDiscoverNew(juce::Graphics &g, juce::Rectangle<int> bounds);
    void drawTopTracks(juce::Graphics &g, juce::Rectangle<int> bounds);
    void drawProgressBar(juce::Graphics &g, juce::Rectangle<int> bounds);
    void createLeftSidebar();
    void createRightSidebar();
    void createMainContent();

    // 네비게이션 메뉴 아이템 구조체
    struct NavMenuItem
    {
        juce::String name;
        juce::String icon;
        bool isSelected = false;
    };

    std::vector<NavMenuItem> navItems = {
        {"Home", "home", true},
        {"Analysis", "analysis"},
        {"Learning", "learning"},
        {"Customiza", "customiza"},
        {"Favorite", "favorite"},
        {"Settings", "settings"},
        {"Sign out", "signout"}};

    // Real-time Analysis 카드 구조체
    struct AnalysisCard
    {
        juce::String title;
        juce::String subtitle;
        juce::Image background;
    };

    std::vector<AnalysisCard> analysisCards;

    // Discover new 카드 구조체
    struct DiscoverCard
    {
        juce::String title;
        juce::String subtitle;
        juce::Image background;
    };

    std::vector<DiscoverCard> discoverCards;

    // Top tracks 아이템 구조체
    struct TrackItem
    {
        juce::String title;
        juce::String artist;
        juce::Image thumbnail;
    };

    std::vector<TrackItem> topTracks;

    // 컴포넌트 멤버 변수들
    std::vector<std::unique_ptr<SidebarButton>> sidebarButtons;
    std::vector<std::unique_ptr<FriendActivityItem>> friendActivities;
    std::vector<std::unique_ptr<SongThumbnail>> recentSongs;
    std::vector<std::unique_ptr<SongThumbnail>> trendingSongs;

    float learningProgress = 0.65f;

    juce::Font titleFont;
    juce::Font regularFont;
    juce::Font smallFont;

    juce::Typeface::Ptr regularTypeface;
    juce::Typeface::Ptr boldTypeface;

    std::function<void(Screen)> screenChangeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomeComponent)
};