#pragma once

#include <JuceHeader.h>

class FriendActivityItem : public juce::Component
{
public:
    FriendActivityItem(const juce::String& name, const juce::String& activity, const juce::String& time);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::String friendName;
    juce::String activity;
    juce::String timeStamp;
    
    juce::Font nameFont { juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::plain };
    juce::Font activityFont { juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::plain };
    juce::Font timeFont { juce::Font::getDefaultSansSerifFontName(), 12.0f, juce::Font::plain };
}; 