#include "SongThumbnail.h"

SongThumbnail::SongThumbnail(const juce::String& songTitle, 
                           const juce::String& songArtist, 
                           const juce::File& imageFile)
    : title(songTitle)
    , artist(songArtist)
{
    if (imageFile.existsAsFile())
    {
        thumbnail = juce::ImageFileFormat::loadFrom(imageFile);
    }

    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void SongThumbnail::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw background
    if (isMouseOver)
    {
        g.setColour(juce::Colour(0xFFE6D5C5));  // Lighter mocha for hover
        g.fillRoundedRectangle(bounds.toFloat(), 10.0f);
    }
    
    // Calculate image and text areas
    auto imageArea = bounds.removeFromTop(bounds.getHeight() - 60);  // 하단에 60픽셀 공간 확보
    auto textArea = bounds;

    // Draw thumbnail
    if (thumbnail.isValid())
    {
        g.drawImage(thumbnail, imageArea.reduced(10).toFloat(), juce::RectanglePlacement::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xFF1A1A1A));
        g.fillRoundedRectangle(imageArea.reduced(10).toFloat(), 5.0f);
    }
    
    // Draw title and artist
    textArea = textArea.reduced(10, 0);  // 좌우 여백
    
    // Title (bold)
    g.setFont(titleFont.boldened());
    g.setColour(juce::Colours::black);
    g.drawText(title, textArea.removeFromTop(30), juce::Justification::centred);
    
    // Artist
    g.setFont(artistFont);
    g.setColour(juce::Colour(0xFF666666));  // Gray for artist name
    g.drawText(artist, textArea, juce::Justification::centred);
}

void SongThumbnail::mouseEnter(const juce::MouseEvent&)
{
    isMouseOver = true;
    repaint();
}

void SongThumbnail::mouseExit(const juce::MouseEvent&)
{
    isMouseOver = false;
    repaint();
} 