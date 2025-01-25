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
        g.setColour(juce::Colour(0xFF2A2A2A));
        g.fillRoundedRectangle(bounds.toFloat(), 10.0f);
    }
    
    // Draw thumbnail
    auto imageBounds = bounds.removeFromTop(bounds.getWidth()).reduced(10);
    if (thumbnail.isValid())
    {
        g.drawImage(thumbnail, imageBounds.toFloat(), juce::RectanglePlacement::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xFF1A1A1A));
        g.fillRoundedRectangle(imageBounds.toFloat(), 5.0f);
    }
    
    bounds.removeFromTop(10);
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(titleFont);
    g.drawText(title, bounds.removeFromTop(20), juce::Justification::centred);
    
    // Draw artist
    g.setColour(juce::Colour(0xFFAAAAAA));
    g.setFont(artistFont);
    g.drawText(artist, bounds.removeFromTop(20), juce::Justification::centred);
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