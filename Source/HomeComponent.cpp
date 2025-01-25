#include "HomeComponent.h"

HomeComponent::HomeComponent(std::function<void(Screen)> callback)
    : screenChangeCallback(std::move(callback))
{
    // Set up fonts
    titleFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 24.0f, juce::Font::bold);
    regularFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::plain);

    createLeftSidebar();
    createRightSidebar();
    createMainContent();
}

HomeComponent::~HomeComponent()
{
}

void HomeComponent::paint(juce::Graphics& g)
{
    // Fill background with white
    g.fillAll(juce::Colours::white);

    auto bounds = getLocalBounds();
    auto leftSidebar = bounds.removeFromLeft(bounds.getWidth() / 5).reduced(20);
    auto rightSidebar = bounds.removeFromRight(bounds.getWidth() / 4).reduced(20);
    auto mainContent = bounds.reduced(20);

    // Draw sidebars background
    g.setColour(juce::Colour(0xFFFEFAE0));  // Mocha Mousse
    g.fillRoundedRectangle(leftSidebar.toFloat(), 25.0f);
    g.fillRoundedRectangle(rightSidebar.toFloat(), 25.0f);

    // Draw section titles
    g.setFont(titleFont);
    g.setColour(juce::Colour(0xFF333333));  // Dark gray for text

    // Friend Activity title
    auto rightArea = rightSidebar.reduced(25);
    g.drawText("Friend Activity", rightArea.removeFromTop(40), juce::Justification::left);

    // Recent Songs title
    auto contentArea = mainContent.reduced(10);
    auto recentArea = contentArea.removeFromTop(contentArea.getHeight() / 3);
    g.drawText("Recent Songs", recentArea.removeFromTop(40), juce::Justification::left);

    // Progress bar
    auto progressArea = contentArea.removeFromTop(100);
    drawProgressBar(g, progressArea);

    // Trending Songs title
    auto trendingArea = contentArea;
    g.drawText("Trending Songs", trendingArea.removeFromTop(40), juce::Justification::left);
}

void HomeComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Left sidebar (1/5 of width)
    auto leftSidebar = bounds.removeFromLeft(bounds.getWidth() / 5).reduced(20);
    auto buttonArea = leftSidebar.reduced(25);
    
    // Position sidebar buttons
    int buttonHeight = 50;
    int buttonMargin = 15;
    
    // Logo at top
    sidebarButtons[0]->setBounds(buttonArea.removeFromTop(buttonHeight));
    buttonArea.removeFromTop(20);  // Extra space after logo
    
    // Main buttons
    for (size_t i = 1; i < sidebarButtons.size() - 1; ++i)
    {
        sidebarButtons[i]->setBounds(buttonArea.removeFromTop(buttonHeight));
        buttonArea.removeFromTop(buttonMargin);
    }
    
    // Logout button at bottom
    sidebarButtons.back()->setBounds(buttonArea.removeFromBottom(buttonHeight));

    // Right sidebar (1/4 of width)
    auto rightSidebar = bounds.removeFromRight(bounds.getWidth() / 4).reduced(20);
    auto activityArea = rightSidebar.reduced(25);
    
    // Skip title area
    activityArea.removeFromTop(60);  // Title + margin

    // Position friend activities
    int activityHeight = 80;
    int activityMargin = 10;
    for (auto& activity : friendActivities)
    {
        activity->setBounds(activityArea.removeFromTop(activityHeight));
        activityArea.removeFromTop(activityMargin);
    }

    // Main content area
    auto mainContent = bounds.reduced(10);
    auto contentArea = mainContent.reduced(20);
    
    // Recent songs section
    auto recentSongsArea = contentArea.removeFromTop(contentArea.getHeight() / 3);
    recentSongsArea.removeFromTop(60);  // Title + margin

    // Layout recent songs in a grid
    int thumbnailSize = 200;
    int thumbnailHeight = 260;  // ?’ì´ë¥?ì¦ê??œì¼œ ?ìŠ¤??ê³µê°„ ?•ë³´
    int thumbnailMargin = 20;
    int columns = recentSongsArea.getWidth() / (thumbnailSize + thumbnailMargin);
    int x = 0, y = 0;
    
    for (auto& song : recentSongs)
    {
        song->setBounds(recentSongsArea.getX() + x * (thumbnailSize + thumbnailMargin),
                       recentSongsArea.getY() + y,
                       thumbnailSize,
                       thumbnailHeight);  // ?’ì´ ë³€ê²?
        
        x++;
        if (x >= columns)
        {
            x = 0;
            y += thumbnailHeight + thumbnailMargin;
        }
    }
    
    // Progress bar area
    auto progressArea = contentArea.removeFromTop(100);
    
    // Trending songs section
    auto trendingArea = contentArea;
    trendingArea.removeFromTop(60);  // Title + margin

    // Layout trending songs in a grid
    x = 0;
    y = 0;
    
    for (auto& song : trendingSongs)
    {
        song->setBounds(trendingArea.getX() + x * (thumbnailSize + thumbnailMargin),
                       trendingArea.getY() + y,
                       thumbnailSize,
                       thumbnailHeight);  // ?’ì´ ë³€ê²?
        
        x++;
        if (x >= columns)
        {
            x = 0;
            y += thumbnailHeight + thumbnailMargin;
        }
    }
}

void HomeComponent::createLeftSidebar()
{
    auto addButton = [this](const juce::String& name, const juce::String& iconPath) {
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
    auto addActivity = [this](const juce::String& name, const juce::String& activity, const juce::String& time) {
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
    auto addSong = [this](const juce::String& title, const juce::String& artist, const juce::String& imagePath, bool isRecent) {
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

void HomeComponent::drawProgressBar(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto progressBounds = bounds.reduced(20);
    
    // Background
    g.setColour(juce::Colour(0xFF2A2A2A));
    g.fillRoundedRectangle(progressBounds.toFloat(), 10.0f);
    
    // Progress
    g.setColour(juce::Colour(0xFFFEFAE0));  // Spotify green
    g.fillRoundedRectangle(progressBounds.withWidth(progressBounds.getWidth() * learningProgress).toFloat(), 10.0f);
    
    // Progress text
    g.setFont(regularFont);
    g.setColour(juce::Colours::white);
    g.drawText(juce::String(static_cast<int>(learningProgress * 100)) + "% Complete",
               progressBounds,
               juce::Justification::centred);
} 