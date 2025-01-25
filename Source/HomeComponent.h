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

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void createLeftSidebar();
    void createRightSidebar();
    void createMainContent();
    void drawProgressBar(juce::Graphics& g, juce::Rectangle<int> bounds);

    // Left sidebar
    std::vector<std::unique_ptr<SidebarButton>> sidebarButtons;
    
    // Right sidebar
    std::vector<std::unique_ptr<FriendActivityItem>> friendActivities;
    
    // Main content
    std::vector<std::unique_ptr<SongThumbnail>> recentSongs;
    std::vector<std::unique_ptr<SongThumbnail>> trendingSongs;
    float learningProgress = 0.65f;  // Example progress value

    // Fonts
    juce::Font titleFont;
    juce::Font regularFont;
    
    std::function<void(Screen)> screenChangeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HomeComponent)
}; 