#pragma once

#include <JuceHeader.h>

struct SongInfo
{
    juce::String title;
    juce::String artist;
    juce::File albumArtFile;
    juce::File audioFile;
};

class SongSelectionComponent : public juce::Component
{
public:
    SongSelectionComponent(std::function<void(const SongInfo&)> onSongSelected);
    ~SongSelectionComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Grid layout properties
    static constexpr int thumbnailSize = 180;
    static constexpr int thumbnailPadding = 15;
    static constexpr int gridSpacing = 40;
    static constexpr int columns = 3;

    class SongThumbnail : public juce::Component
    {
    public:
        SongThumbnail(const SongInfo& info, std::function<void()> onClick);
        void paint(juce::Graphics&) override;
        void mouseEnter(const juce::MouseEvent& event) override;
        void mouseExit(const juce::MouseEvent& event) override;
        void mouseDown(const juce::MouseEvent& event) override;
        void mouseUp(const juce::MouseEvent& event) override;

    private:
        SongInfo songInfo;
        std::function<void()> onClick;
        bool isMouseOver = false;
        bool isMouseDown = false;
        juce::Image albumArt;
    };

    void loadSongs();
    void createThumbnails();

    std::vector<SongInfo> songs;
    std::vector<std::unique_ptr<SongThumbnail>> thumbnails;
    std::function<void(const SongInfo&)> songSelectedCallback;

    juce::File resourcesFolder;
    juce::File getSongsFolder() const;
    juce::File getAlbumArtFolder() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SongSelectionComponent)
}; 