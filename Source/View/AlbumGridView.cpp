#include "AlbumGridView.h"

AlbumGridView::AlbumGridView()
{
    // Setup default properties
    numColumns = 3;
    thumbnailSize = 150;
    spacing = 10;
}

void AlbumGridView::addAlbum(const juce::String& title, const juce::Image& thumbnail)
{
    auto* newItem = new AlbumThumbnailComponent(title, thumbnail);
    addAndMakeVisible(newItem);
    thumbnails.add(newItem);
    resized();
}

void AlbumGridView::addAlbumFromSpotify(const SpotifyService::Album& album)
{
    auto* newItem = new AlbumThumbnailComponent(album.name);
    newItem->setAlbumInfo(album);
    addAndMakeVisible(newItem);
    thumbnails.add(newItem);
    resized();
}

void AlbumGridView::addSong(const Song& song, std::function<void(const juce::String&)> onSongSelected)
{
    auto* newItem = new AlbumThumbnailComponent(song.getTitle());
    newItem->setSongInfo(song);
    
    // 클릭 이벤트 핸들러 설정
    newItem->onClick = [this, newItem, onSongSelected]() {
        juce::String songId = newItem->getSongId();
        if (!songId.isEmpty() && onSongSelected)
            onSongSelected(songId);
    };
    
    addAndMakeVisible(newItem);
    thumbnails.add(newItem);
    resized();
}

void AlbumGridView::clear()
{
    thumbnails.clear();
}

void AlbumGridView::setNumColumns(int columns)
{
    numColumns = juce::jmax(1, columns);
    resized();
}

void AlbumGridView::setThumbnailSize(int size)
{
    thumbnailSize = juce::jmax(50, size);
    resized();
}

void AlbumGridView::setSpacing(int newSpacing)
{
    spacing = juce::jmax(0, newSpacing);
    resized();
}

void AlbumGridView::resized()
{
    auto bounds = getLocalBounds();
    
    // Calculate item width including spacing
    int itemWidth = thumbnailSize;
    int itemHeight = thumbnailSize + 30; // Adding space for text
    
    int row = 0;
    int col = 0;
    
    for (auto* thumbnail : thumbnails)
    {
        thumbnail->setBounds(col * (itemWidth + spacing), 
                            row * (itemHeight + spacing),
                            itemWidth, itemHeight);
        
        // Move to next column or row
        col++;
        if (col >= numColumns)
        {
            col = 0;
            row++;
        }
    }
    
    // Set component size based on content
    int minHeight = thumbnails.isEmpty() ? 0 : 
                    ((thumbnails.size() - 1) / numColumns + 1) * (itemHeight + spacing) - spacing;
    setSize(getWidth(), minHeight);
}