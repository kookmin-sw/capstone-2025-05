#pragma once

#include <JuceHeader.h>

class SongThumbnail : public juce::Component
{
public:
    SongThumbnail(const juce::String& title, const juce::String& artist, const juce::File& imageFile);
    void paint(juce::Graphics& g) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

private:
    juce::String title;
    juce::String artist;
    juce::Image thumbnail;
    bool isMouseOver = false;
    
    juce::Font titleFont { juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::plain };
    juce::Font artistFont { juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::plain };
}; 