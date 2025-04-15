#pragma once
#include <JuceHeader.h>

// 노래(악보) 데이터 모델
class Song
{
public:
    Song(const juce::String& id, const juce::String& title, const juce::String& artist, const juce::String& thumbnailPath)
        : id(id), title(title), artist(artist), thumbnailPath(thumbnailPath)
    {
    }
    
    juce::String getId() const { return id; }
    juce::String getTitle() const { return title; }
    juce::String getArtist() const { return artist; }
    juce::String getThumbnailPath() const { return thumbnailPath; }
    
private:
    juce::String id;
    juce::String title;
    juce::String artist;
    juce::String thumbnailPath;
};