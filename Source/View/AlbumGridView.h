#pragma once
#include <JuceHeader.h>
#include "AlbumThumbnailComponent.h"
#include "../Model/Song.h"
#include "../Model/SpotifyService.h"

class ContentController;

// 앨범 그리드 뷰 컴포넌트
class AlbumGridView : public juce::Component
{
public:
    AlbumGridView();
    
    void addAlbum(const juce::String& title, const juce::Image& thumbnail = juce::Image());
    void addAlbumFromSpotify(const SpotifyService::Album& album);
    void addSong(const Song& song, std::function<void(const juce::String&)> onSongSelected);
    void clear();
    
    // ContentController 설정
    void setContentController(ContentController* controller);
    
    void setNumColumns(int columns);
    void setThumbnailSize(int size);
    void setSpacing(int newSpacing);
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::OwnedArray<AlbumThumbnailComponent> thumbnails;
    int numColumns;
    int thumbnailSize;
    int spacing;
    ContentController* contentController = nullptr;
};