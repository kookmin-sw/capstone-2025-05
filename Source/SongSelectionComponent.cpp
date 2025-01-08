#include "SongSelectionComponent.h"

SongSelectionComponent::SongSelectionComponent(std::function<void(const SongInfo&)> onSongSelected)
    : songSelectedCallback(std::move(onSongSelected))
{
    // Initialize audio device
    deviceManager.initialiseWithDefaultDevices(0, 2);
    
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
    const int maxElevation = 8;  // ?? ??? ??
    
    // Calculate total grid padding with top margin for elevation
    int horizontalPadding = gridSpacing * 2;
    int topPadding = gridSpacing * 2 + maxElevation;  // ??? ?? ?? ??
    
    // Calculate grid layout with different top/side padding
    auto bounds = getLocalBounds();
    bounds.removeFromTop(topPadding);  // ?? ?? ??
    bounds.reduce(horizontalPadding, 0);  // ?? ??
    
    int row = 0;
    int col = 0;

    // Calculate grid metrics
    int availableWidth = bounds.getWidth();
    int effectiveWidth = (availableWidth - (gridSpacing * (columns - 1))) / columns;
    int actualThumbnailSize = std::min(thumbnailSize, effectiveWidth);
    
    // Center the grid
    int totalGridWidth = (columns * actualThumbnailSize) + ((columns - 1) * gridSpacing);
    int startX = bounds.getX() + (availableWidth - totalGridWidth) / 2;

    for (auto& thumbnail : thumbnails)
    {
        int x = startX + (col * (actualThumbnailSize + gridSpacing));
        int y = bounds.getY() + (row * (actualThumbnailSize + gridSpacing));

        thumbnail->setBounds(x, y, actualThumbnailSize, actualThumbnailSize);

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
        },
        {
            "Sunfish", 
            "Younha", 
            albumArtFolder.getChildFile("Sunfish.jpg"),
            songsFolder.getChildFile("Sunfish.mp3")
        },
        // ... more songs
    };
}

void SongSelectionComponent::createThumbnails()
{
    for (const auto& song : songs)
    {
        auto thumbnail = std::make_unique<SongThumbnail>(song, [this, song]() {
            songSelectedCallback(song);
        }, deviceManager);
        addAndMakeVisible(thumbnail.get());
        thumbnails.push_back(std::move(thumbnail));
    }
}

// SongThumbnail implementation
SongSelectionComponent::SongThumbnail::SongThumbnail(
    const SongInfo& info, 
    std::function<void()> onClick,
    juce::AudioDeviceManager& deviceManager)
    : songInfo(info)
    , onClick(std::move(onClick))
    , audioDeviceManager(deviceManager)
{
    // Initialize audio format manager
    formatManager = std::make_unique<juce::AudioFormatManager>();
    formatManager->registerBasicFormats();

    // Initialize audio source player
    audioSourcePlayer = std::make_unique<juce::AudioSourcePlayer>();
    audioDeviceManager.addAudioCallback(audioSourcePlayer.get());

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

SongSelectionComponent::SongThumbnail::~SongThumbnail()
{
    stopTimer();
    stopPreview();
    audioDeviceManager.removeAudioCallback(audioSourcePlayer.get());
}

void SongSelectionComponent::SongThumbnail::paint(juce::Graphics& g)
{
    float elevation = isMouseDown ? 2.0f : (isMouseOver ? 8.0f : 4.0f);
    
    // ?? ???? ??? elevation?? ??? ? ??
    auto bounds = getLocalBounds().toFloat().reduced(thumbnailPadding).withTrimmedTop(elevation);
    
    // Calculate content bounds
    auto contentBounds = bounds;
    if (isMouseOver)
    {
        contentBounds = contentBounds.translated(0, -elevation);
    }

    // Draw shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(bounds.expanded(elevation), 4.0f);

    // Draw card background with hover effect
    if (isMouseOver)
    {
        g.setColour(juce::Colour(0xFF3D3D3D));
    }
    else
    {
        g.setColour(juce::Colour(0xFF2D2D2D));
    }
    g.fillRoundedRectangle(contentBounds, 4.0f);

    // Draw album art or placeholder
    auto imageArea = contentBounds.withTrimmedBottom(30.0f).reduced(8);
    if (albumArt.isValid())
    {
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

    // Draw title and artist
    auto textArea = contentBounds.removeFromBottom(30.0f);
    
    // Draw artist name
    g.setFont(12.0f);
    g.setColour(juce::Colours::white.withAlpha(0.7f));  // ?„í‹°?¤íŠ¸ ?´ë¦„?€ ?½ê°„ ?¬ëª…?˜ê²Œ
    g.drawText(songInfo.artist, textArea.removeFromTop(12.0f),
               juce::Justification::centred, true);
               
    // Draw title
    g.setFont(14.0f);
    g.setColour(juce::Colours::white);  // ?œëª©?€ ?„ì „ ë¶ˆíˆ¬ëª…í•˜ê²?
    g.drawText(songInfo.title, textArea,
               juce::Justification::centred, true);

    // Add subtle highlight effect on hover
    if (isMouseOver)
    {
        g.setGradientFill(juce::ColourGradient(
            juce::Colours::white.withAlpha(0.1f),
            contentBounds.getTopLeft(),
            juce::Colours::transparentWhite,
            contentBounds.getBottomRight(),
            true));
        g.fillRoundedRectangle(contentBounds, 4.0f);
    }
}

void SongSelectionComponent::SongThumbnail::mouseEnter(const juce::MouseEvent&)
{
    isMouseOver = true;
    startTimer(previewDelay);  // Start timer for preview delay
    repaint();
}

void SongSelectionComponent::SongThumbnail::mouseExit(const juce::MouseEvent&)
{
    isMouseOver = false;
    stopTimer();
    stopPreview();
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

void SongSelectionComponent::SongThumbnail::timerCallback()
{
    stopTimer();
    if (isMouseOver)
    {
        startPreview();
    }
}

void SongSelectionComponent::SongThumbnail::startPreview()
{
    if (!transportSource)
    {
        transportSource = std::make_unique<juce::AudioTransportSource>();

        // Debug: Print file path and existence
        DBG("Trying to load audio file: " + songInfo.audioFile.getFullPathName());
        DBG("File exists: " + juce::String(songInfo.audioFile.existsAsFile() ? "yes" : "no"));

        if (auto* reader = formatManager->createReaderFor(songInfo.audioFile))
        {
            DBG("Successfully created reader for: " + songInfo.title);
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            transportSource->setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
            
            const double thirtySeconds = 30.0;
            const double startPosition = std::min(thirtySeconds, transportSource->getLengthInSeconds() / 2);
            transportSource->setPosition(startPosition);

            audioSourcePlayer->setSource(transportSource.get());
        }
        else
        {
            DBG("Failed to create reader for: " + songInfo.title);
        }
    }

    if (transportSource)
    {
        transportSource->start();
    }
}

void SongSelectionComponent::SongThumbnail::stopPreview()
{
    if (transportSource)
    {
        transportSource->stop();
        audioSourcePlayer->setSource(nullptr);
        transportSource->setSource(nullptr);
        transportSource.reset();
        readerSource.reset();
    }
} 