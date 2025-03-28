#include "ContentPanelComponent.h"

// Example of how ContentPanelComponent.cpp might need to be updated
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
    
    // Add some sample data
    initializeSampleData();
}

void ContentPanelComponent::initializeSampleData()
{
    // Add sample albums to recent grid
    recentGrid.addAlbum(juce::String::fromUTF8("곡 A"));
    recentGrid.addAlbum(juce::String::fromUTF8("곡 B"));
    recentGrid.addAlbum(juce::String::fromUTF8("곡 C"));
    
    // Add sample albums to recommend grid
    recommendGrid.addAlbum(juce::String::fromUTF8("곡 D"));
    recommendGrid.addAlbum(juce::String::fromUTF8("곡 E"));
    recommendGrid.addAlbum(juce::String::fromUTF8("곡 F"));
}

void ContentPanelComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Layout for recent section
    recentTitle.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(5); // spacing
    
    auto recentSection = bounds.removeFromTop(200);
    recentGrid.setBounds(recentSection.withTrimmedRight(100));
    viewAllRecent.setBounds(recentSection.removeFromRight(90).withTrimmedTop(5));
    
    bounds.removeFromTop(20); // spacing between sections
    
    // Layout for recommended section
    recommendTitle.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(5); // spacing
    
    auto recommendSection = bounds.removeFromTop(200);
    recommendGrid.setBounds(recommendSection.withTrimmedRight(100));
    viewAllRecommend.setBounds(recommendSection.removeFromRight(90).withTrimmedTop(5));
}