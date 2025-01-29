#include "HomeComponent.h"

HomeComponent::HomeComponent(std::function<void(Screen)> callback)
    : screenChangeCallback(std::move(callback))
{
    // ?? ??
    auto fontFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getChildFile("Resources/Fonts/Montserrat-Regular.ttf");

    auto boldFontFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                            .getParentDirectory()
                            .getParentDirectory()
                            .getParentDirectory()
                            .getParentDirectory()
                            .getParentDirectory()
                            .getParentDirectory()
                            .getChildFile("Resources/Fonts/Montserrat-Bold.ttf");

    // ?? ??
    if (fontFile.existsAsFile() && boldFontFile.existsAsFile())
    {
        juce::MemoryBlock regularMemoryBlock;
        juce::MemoryBlock boldMemoryBlock;

        fontFile.loadFileAsData(regularMemoryBlock);
        boldFontFile.loadFileAsData(boldMemoryBlock);

        regularTypeface = juce::Typeface::createSystemTypefaceFor(regularMemoryBlock.getData(),
                                                                  regularMemoryBlock.getSize());
        boldTypeface = juce::Typeface::createSystemTypefaceFor(boldMemoryBlock.getData(),
                                                               boldMemoryBlock.getSize());

        titleFont = juce::Font(boldTypeface).withHeight(24.0f);
        regularFont = juce::Font(regularTypeface).withHeight(16.0f);
        smallFont = juce::Font(regularTypeface).withHeight(14.0f);
    }
    else
    {
        // ?? ??? ?? ?? ?? ?? ??
        titleFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 24.0f, juce::Font::bold);
        regularFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::plain);
        smallFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 14.0f, juce::Font::plain);
    }

    // Real-time Analysis ?? ???
    analysisCards = {
        {"Relaxing", "Technique", {}},
        {"Smooth", "Weekend", {}},
        {"Relaxation", "Rock Riffs", {}},
        {"Classic", "Masterpiece", {}}};

    // Discover new ?? ???
    discoverCards = {
        {"Top Picks", "Techniques", {}},
        {"Essentials", "Instrumental", {}},
        {"Your Journey", "Start Here", {}}};

    // Top tracks ???
    topTracks = {
        {"Musician's life", "David", {}},
        {"Hit Play", "Ella", {}},
        {"Harmon", "Chords progression", {}}};

    // ??? ??
    auto loadImage = [](const juce::String &name) -> juce::Image
    {
        auto imageFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                             .getParentDirectory()
                             .getParentDirectory()
                             .getParentDirectory()
                             .getParentDirectory()
                             .getParentDirectory()
                             .getParentDirectory()
                             .getChildFile("Resources/Images/" + name + ".png");

        if (imageFile.existsAsFile())
            return juce::ImageFileFormat::loadFrom(imageFile);
        return {};
    };

    // ? ??? ?? ??? ??
    for (size_t i = 0; i < analysisCards.size(); ++i)
        analysisCards[i].background = loadImage("analysis_" + juce::String(i + 1));

    for (size_t i = 0; i < discoverCards.size(); ++i)
        discoverCards[i].background = loadImage("discover_" + juce::String(i + 1));

    for (size_t i = 0; i < topTracks.size(); ++i)
        topTracks[i].thumbnail = loadImage("track_" + juce::String(i + 1));

    createLeftSidebar();
    createRightSidebar();
    createMainContent();
}

HomeComponent::~HomeComponent()
{
}

void HomeComponent::paint(juce::Graphics &g)
{
    // ?? ??
    g.fillAll(juce::Colour(0xFFF5F5F5));

    auto bounds = getLocalBounds();

    // ?? ????? ??
    auto navBounds = bounds.removeFromLeft(250);
    drawNavigationMenu(g);

    // ?? ??? ??
    drawMainContent(g);
}

void HomeComponent::drawNavigationMenu(juce::Graphics &g)
{
    auto bounds = getLocalBounds().removeFromLeft(250);

    // ????? ??
    g.setColour(juce::Colour(0xFFE6D5C5));
    g.fillRect(bounds);

    // GuitarPr ??
    g.setFont(titleFont);
    g.setColour(juce::Colours::black);
    auto logoBounds = bounds.removeFromTop(100);
    g.drawText("GuitarPr", logoBounds.reduced(20), juce::Justification::centredLeft);

    // ????? ????
    bounds.removeFromTop(20);
    auto itemBounds = bounds.reduced(20, 0);

    for (const auto &item : navItems)
    {
        auto itemHeight = 50;
        auto currentBounds = itemBounds.removeFromTop(itemHeight);

        if (item.isSelected)
        {
            g.setColour(juce::Colour(0xFFD9C5B2));
            g.fillRoundedRectangle(currentBounds.toFloat(), 10.0f);
        }

        g.setColour(juce::Colours::black);
        g.setFont(regularFont);
        g.drawText(item.name, currentBounds.reduced(40, 0), juce::Justification::centredLeft);

        itemBounds.removeFromTop(10); // ??
    }
}

void HomeComponent::drawMainContent(juce::Graphics &g)
{
    auto bounds = getLocalBounds().withTrimmedLeft(250).reduced(40);

    // Real-time Analysis ??
    g.setFont(titleFont);
    g.setColour(juce::Colours::black);
    auto analysisBounds = bounds.removeFromTop(300);
    g.drawText("Real-time Analysis", analysisBounds.removeFromTop(40), juce::Justification::left);
    drawRealTimeAnalysis(g, analysisBounds);

    bounds.removeFromTop(40); // ??

    // Discover new ??
    g.setFont(titleFont);
    auto discoverBounds = bounds.removeFromTop(300);
    g.drawText("Discover new", discoverBounds.removeFromTop(40), juce::Justification::left);
    drawDiscoverNew(g, discoverBounds);

    bounds.removeFromTop(40); // ??

    // Top tracks ??
    auto tracksBounds = bounds;
    g.drawText("Top tracks", tracksBounds.removeFromTop(40), juce::Justification::left);
    drawTopTracks(g, tracksBounds);
}

void HomeComponent::drawRealTimeAnalysis(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    int cardWidth = (bounds.getWidth() - 30) / 4; // 4?? ??? ??
    auto cardBounds = bounds.removeFromLeft(cardWidth);

    for (const auto &card : analysisCards)
    {
        // ?? ??
        if (card.background.isValid())
        {
            g.drawImage(card.background, cardBounds.toFloat(),
                        juce::RectanglePlacement::fillDestination);
        }
        else
        {
            g.setColour(juce::Colour(0xFFE6D5C5));
            g.fillRoundedRectangle(cardBounds.toFloat(), 10.0f);
        }

        // ?? ???
        auto textBounds = cardBounds.reduced(20);
        g.setColour(juce::Colours::white);
        g.setFont(regularFont);
        g.drawText(card.title, textBounds.removeFromBottom(60), juce::Justification::centred);
        g.setFont(smallFont);
        g.drawText(card.subtitle, textBounds.removeFromBottom(30), juce::Justification::centred);

        bounds.removeFromLeft(10);
        cardBounds = bounds.removeFromLeft(cardWidth);
    }
}

void HomeComponent::drawDiscoverNew(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    int cardWidth = (bounds.getWidth() - 20) / 3; // 3?? ??? ??
    auto cardBounds = bounds.removeFromLeft(cardWidth);

    for (const auto &card : discoverCards)
    {
        // ?? ??
        if (card.background.isValid())
        {
            g.drawImage(card.background, cardBounds.toFloat(),
                        juce::RectanglePlacement::fillDestination);
        }
        else
        {
            g.setColour(juce::Colour(0xFFE6D5C5));
            g.fillRoundedRectangle(cardBounds.toFloat(), 10.0f);
        }

        // ?? ???
        auto textBounds = cardBounds.reduced(20);
        g.setColour(juce::Colours::white);
        g.setFont(regularFont);
        g.drawText(card.title, textBounds.removeFromBottom(60), juce::Justification::centred);
        g.setFont(smallFont);
        g.drawText(card.subtitle, textBounds.removeFromBottom(30), juce::Justification::centred);

        bounds.removeFromLeft(10);
        cardBounds = bounds.removeFromLeft(cardWidth);
    }
}

void HomeComponent::drawTopTracks(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    auto itemBounds = bounds;
    int itemHeight = 70;

    for (const auto &track : topTracks)
    {
        auto currentBounds = itemBounds.removeFromTop(itemHeight);

        // ???
        auto thumbnailBounds = currentBounds.removeFromLeft(itemHeight).reduced(5);
        if (track.thumbnail.isValid())
        {
            g.drawImage(track.thumbnail, thumbnailBounds.toFloat(),
                        juce::RectanglePlacement::centred);
        }
        else
        {
            g.setColour(juce::Colour(0xFFE6D5C5));
            g.fillRoundedRectangle(thumbnailBounds.toFloat(), 5.0f);
        }

        // ?? ??
        currentBounds.removeFromLeft(20);
        g.setColour(juce::Colours::black);
        g.setFont(regularFont);
        g.drawText(track.title, currentBounds.removeFromTop(35), juce::Justification::centredLeft);
        g.setFont(smallFont);
        g.setColour(juce::Colours::grey);
        g.drawText(track.artist, currentBounds, juce::Justification::centredLeft);

        itemBounds.removeFromTop(10); // ??
    }
}

void HomeComponent::resized()
{
    repaint();
}

void HomeComponent::createLeftSidebar()
{
    auto addButton = [this](const juce::String &name, const juce::String &iconPath)
    {
        auto button = std::make_unique<SidebarButton>(name, iconPath);
        addAndMakeVisible(*button);
        sidebarButtons.push_back(std::move(button));
    };

    // Add sidebar buttons
    addButton("MAPLE", "Resources/Icons/record.png");
    addButton("Record", "Resources/Icons/record.png");
    addButton("Playback", "Resources/Icons/playback.png");
    addButton("Settings", "Resources/Icons/settings.png");
    addButton("Profile", "Resources/Icons/profile.png");
    addButton("Download", "Resources/Icons/download.png");
    addButton("Log out", "Resources/Icons/logout.png");
}

void HomeComponent::createRightSidebar()
{
    // Example friend activities
    auto addActivity = [this](const juce::String &name, const juce::String &activity, const juce::String &time)
    {
        auto item = std::make_unique<FriendActivityItem>(name, activity, time);
        addAndMakeVisible(*item);
        friendActivities.push_back(std::move(item));
    };

    addActivity("John Doe", "Started practicing Piano Sonata No.14", "2 minutes ago");
    addActivity("Jane Smith", "Completed daily practice goal", "15 minutes ago");
    addActivity("Mike Johnson", "Achieved new high score", "1 hour ago");
}

void HomeComponent::createMainContent()
{
    // Example recent songs
    auto addSong = [this](const juce::String &title, const juce::String &artist, const juce::String &imagePath, bool isRecent)
    {
        auto albumArtFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                                .getParentDirectory()
                                .getParentDirectory()
                                .getParentDirectory()
                                .getParentDirectory()
                                .getParentDirectory()
                                .getParentDirectory()
                                .getChildFile("Resources/AlbumArt/" + imagePath);

        auto song = std::make_unique<SongThumbnail>(title, artist, albumArtFile);
        addAndMakeVisible(*song);
        if (isRecent)
            recentSongs.push_back(std::move(song));
        else
            trendingSongs.push_back(std::move(song));
    };

    // Add recent songs with album art
    addSong("26", "Younha", "26.jpg", true);
    addSong("Sunfish", "Younha", "sunfish.jpg", true);
    addSong("T.B.H", "QWER", "tbh.jpg", true);

    // Add trending songs with album art
    addSong("Symphony No.5", "Beethoven", "symphony_5.jpg", false);
    addSong("The Four Seasons", "Vivaldi", "four_seasons.jpg", false);
    addSong("Nocturne Op.9 No.2", "Chopin", "nocturne.jpg", false);
}

void HomeComponent::drawProgressBar(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    auto progressBounds = bounds.reduced(20);

    // Background
    g.setColour(juce::Colour(0xFF2A2A2A));
    g.fillRoundedRectangle(progressBounds.toFloat(), 10.0f);

    // Progress
    g.setColour(juce::Colour(0xFFFEFAE0)); // Spotify green
    g.fillRoundedRectangle(progressBounds.withWidth(progressBounds.getWidth() * learningProgress).toFloat(), 10.0f);

    // Progress text
    g.setFont(regularFont);
    g.setColour(juce::Colours::white);
    g.drawText(juce::String(static_cast<int>(learningProgress * 100)) + "% Complete",
               progressBounds,
               juce::Justification::centred);
}