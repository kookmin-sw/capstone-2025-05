#include "AlbumGridView.h"
#include "../Controller/ContentController.h"

AlbumGridView::AlbumGridView()
{
    // Setup default properties
    numColumns = 6;
    thumbnailSize = 180;
    spacing = 15;
}

void AlbumGridView::addAlbum(const juce::String& title, const juce::Image& thumbnail)
{
    auto* newItem = new AlbumThumbnailComponent(title, thumbnail);
    
    // ContentController 설정
    if (contentController != nullptr)
        newItem->setContentController(contentController);
        
    addAndMakeVisible(newItem);
    thumbnails.add(newItem);
    resized();
}

void AlbumGridView::addAlbumFromSpotify(const SpotifyService::Album& album)
{
    auto* newItem = new AlbumThumbnailComponent(album.name);
    newItem->setAlbumInfo(album);
    
    // ContentController 설정
    if (contentController != nullptr)
        newItem->setContentController(contentController);
        
    addAndMakeVisible(newItem);
    thumbnails.add(newItem);
    resized();
}

void AlbumGridView::addSong(const Song& song, std::function<void(const juce::String&)> onSongSelected)
{
    DBG("AlbumGridView::addSong - start: " + song.getTitle() + " (ID: " + song.getId() + ")");
    
    auto* newItem = new AlbumThumbnailComponent(song.getTitle());
    newItem->setSongInfo(song);
    
    // ContentController 설정
    if (contentController != nullptr)
    {
        DBG("AlbumGridView::addSong - contentController is set");
        newItem->setContentController(contentController);
    }
    else
    {
        DBG("AlbumGridView::addSong - contentController is null!");
    }
    
    // 클릭 이벤트 핸들러 설정
    newItem->onClick = [this, newItem, onSongSelected]() {
        juce::String songId = newItem->getSongId();
        if (!songId.isEmpty() && onSongSelected)
            onSongSelected(songId);
    };
    
    addAndMakeVisible(newItem);
    thumbnails.add(newItem);
    
    DBG("AlbumGridView::addSong - thumbnail count: " + juce::String(thumbnails.size()));
    
    resized();
    DBG("AlbumGridView::addSong - complete");
}

void AlbumGridView::setContentController(ContentController* controller)
{
    contentController = controller;
    
    // 기존 썸네일 컴포넌트들에게 ContentController 설정
    for (auto* thumbnail : thumbnails)
    {
        thumbnail->setContentController(contentController);
    }
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

void AlbumGridView::paint(juce::Graphics& g)
{
    // 배경을 투명하게 설정 (기존 흰색 배경 제거)
    g.fillAll(juce::Colours::transparentBlack);
}

void AlbumGridView::resized()
{
    DBG("AlbumGridView::resized - start, thumbnail count: " + juce::String(thumbnails.size()));
    
    auto bounds = getLocalBounds();
    DBG("AlbumGridView::resized - component size: " + juce::String(bounds.getWidth()) + "x" + juce::String(bounds.getHeight()));
    
    // Calculate item width including spacing
    int itemWidth = thumbnailSize;
    int itemHeight = thumbnailSize + 100; // 높이 증가 (80->100)
    
    int row = 0;
    int col = 0;
    
    for (auto* thumbnail : thumbnails)
    {
        // 간격을 약간 늘려 테두리 없이도 구분되게 함
        thumbnail->setBounds(col * (itemWidth + spacing) + 2, 
                            row * (itemHeight + spacing) + 2,
                            itemWidth - 4, itemHeight - 4);
        
        DBG("AlbumGridView::resized - thumbnail [" + juce::String(col) + "," + juce::String(row) + "] position: " + 
            juce::String(col * (itemWidth + spacing)) + "," + juce::String(row * (itemHeight + spacing)) + 
            " size: " + juce::String(itemWidth) + "x" + juce::String(itemHeight));
        
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
    
    DBG("AlbumGridView::resized - calculated min height: " + juce::String(minHeight));
    setSize(getWidth(), minHeight);
    DBG("AlbumGridView::resized - complete");
}