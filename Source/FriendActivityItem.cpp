#include "FriendActivityItem.h"

FriendActivityItem::FriendActivityItem(const juce::String& name, 
                                     const juce::String& activityText, 
                                     const juce::String& time)
    : friendName(name)
    , activity(activityText)
    , timeStamp(time)
{
}

void FriendActivityItem::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Draw friend name
    g.setColour(juce::Colours::black);
    g.setFont(nameFont);
    g.drawText(friendName, bounds.removeFromTop(20), juce::Justification::topLeft);
    
    // Draw activity
    bounds.removeFromTop(5);
    g.setColour(juce::Colour(0xFFAAAAAA));
    g.setFont(activityFont);
    g.drawText(activity, bounds.removeFromTop(40), juce::Justification::topLeft, true);
    
    // Draw timestamp
    g.setColour(juce::Colour(0xFF666666));
    g.setFont(timeFont);
    g.drawText(timeStamp, bounds, juce::Justification::topLeft);
}

void FriendActivityItem::resized()
{
    // 기본 크기 설정
    setSize(getWidth(), 80);
} 