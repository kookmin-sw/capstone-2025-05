#include "SongSelectionComponent.h"

SongSelectionComponent::SongSelectionComponent(std::function<void(const SongInfo&)> onSongSelected)
    : songSelectedCallback(std::move(onSongSelected))
{
    // Get project root directory
    auto exeFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    auto projectDir = exeFile.getParentDirectory()  // Debug
                            .getParentDirectory()    // x64
                            .getParentDirectory()    // VisualStudio2022
                            .getParentDirectory()    // Builds
                            .getParentDirectory()
                            .getParentDirectory();   // MAPLE (project root)
    
    resourcesFolder = projectDir.getChildFile("Resources");
    
    loadSongs();
    createThumbnails();
}

SongSelectionComponent::~SongSelectionComponent()
{
    // Empty destructor
}

juce::File SongSelectionComponent::getSongsFolder() const
{
    return resourcesFolder.getChildFile("Songs");
}

juce::File SongSelectionComponent::getAlbumArtFolder() const
{
    return resourcesFolder.getChildFile("AlbumArt");
}

void SongSelectionComponent::paint(juce::Graphics& g)
{
    // Material Design background
    g.fillAll(juce::Colour(0xFF121212));
}

void SongSelectionComponent::resized()
{
    // Calculate grid layout
    auto bounds = getLocalBounds().reduced(padding);
    int row = 0;
    int col = 0;

    for (auto& thumbnail : thumbnails)
    {
        int x = bounds.getX() + col * (thumbnailSize + padding);
        int y = bounds.getY() + row * (thumbnailSize + padding);

        thumbnail->setBounds(x, y, thumbnailSize, thumbnailSize);

        col++;
        if (col >= columns)
        {
            col = 0;
            row++;
        }
    }
}

void SongSelectionComponent::loadSongs()
{
    auto songsFolder = getSongsFolder();
    auto albumArtFolder = getAlbumArtFolder();
    
    songs = {
        {
            "26", 
            "Younha", 
            albumArtFolder.getChildFile("26.jpg"),
            songsFolder.getChildFile("26.mp3")
        }
        // ... more songs
    };
}

void SongSelectionComponent::createThumbnails()
{
    for (const auto& song : songs)
    {
        auto thumbnail = std::make_unique<SongThumbnail>(song, [this, song]() {
            songSelectedCallback(song);
        });
        addAndMakeVisible(thumbnail.get());
        thumbnails.push_back(std::move(thumbnail));
    }
}

// SongThumbnail implementation
SongSelectionComponent::SongThumbnail::SongThumbnail(const SongInfo& info, std::function<void()> onClick)
    : songInfo(info), onClick(std::move(onClick))
{
    if (songInfo.albumArtFile.existsAsFile())
    {
        albumArt = juce::ImageFileFormat::loadFrom(songInfo.albumArtFile);
    }
    
    if (!albumArt.isValid())
    {
        // Create placeholder image if loading fails
        albumArt = juce::Image(juce::Image::RGB, thumbnailSize, thumbnailSize, true);
        juce::Graphics g(albumArt);
        g.fillAll(juce::Colour(0xFF1E1E1E));
        g.setColour(juce::Colours::white);
        g.drawText("No Image", albumArt.getBounds(), juce::Justification::centred, true);
    }
}

void SongSelectionComponent::SongThumbnail::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float elevation = isMouseDown ? 2.0f : (isMouseOver ? 8.0f : 4.0f);

    // Draw shadow with lighter color
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.drawRoundedRectangle(bounds.translated(0, elevation).expanded(2),
                          4.0f, elevation * 2.0f);

    // Draw card background
    g.setColour(juce::Colour(0xFF2D2D2D));  // Slightly lighter background
    g.fillRoundedRectangle(bounds, 4.0f);

    // Draw album art or placeholder
    auto imageArea = bounds.withTrimmedBottom(30.0f).reduced(8);  // Add padding
    if (albumArt.isValid())
    {
        // Draw with better quality settings and fit entire image
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(albumArt, 
                   imageArea, 
                   juce::RectanglePlacement::centred | juce::RectanglePlacement::fillDestination);
    }
    else
    {
        g.setColour(juce::Colour(0xFF1E1E1E));
        g.fillRoundedRectangle(imageArea, 4.0f);
    }

    // Draw title and artist with improved visibility
    auto textArea = bounds.removeFromBottom(30.0f);
    g.setColour(juce::Colours::white);
    
    // Draw artist name
    g.setFont(12.0f);
    g.drawText(songInfo.artist, textArea.removeFromTop(12.0f),
               juce::Justification::centred, true);
               
    // Draw title with slightly larger font
    g.setFont(14.0f);
    g.setColour(juce::Colours::white.withAlpha(0.9f));  // Slightly transparent for hierarchy
    g.drawText(songInfo.title, textArea,
               juce::Justification::centred, true);

    // Draw hover effect
    if (isMouseOver)
    {
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.fillRoundedRectangle(bounds, 4.0f);
    }
}

void SongSelectionComponent::SongThumbnail::mouseEnter(const juce::MouseEvent&)
{
    isMouseOver = true;
    repaint();
}

void SongSelectionComponent::SongThumbnail::mouseExit(const juce::MouseEvent&)
{
    isMouseOver = false;
    repaint();
}

void SongSelectionComponent::SongThumbnail::mouseDown(const juce::MouseEvent&)
{
    isMouseDown = true;
    repaint();
}

void SongSelectionComponent::SongThumbnail::mouseUp(const juce::MouseEvent& event)
{
    isMouseDown = false;
    if (event.getNumberOfClicks() == 1 && contains(event.getPosition()))
        onClick();
    repaint();
} 