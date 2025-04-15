#pragma once
#include <JuceHeader.h>
#include "SpotifyService.h"
#include "Model/Song.h"

class MainComponent;  // 전방 선언

class ContentPanelComponent : public juce::Component
{
public:
    ContentPanelComponent(MainComponent& mainComp);
    void resized() override;
    
    // 곡 선택 이벤트 리스너 인터페이스
    class SongSelectedListener
    {
    public:
        virtual ~SongSelectedListener() = default;
        virtual void songSelected(const juce::String& songId) = 0;
    };
    
    void addSongSelectedListener(SongSelectedListener* listener) { songListeners.add(listener); }
    void removeSongSelectedListener(SongSelectedListener* listener) { songListeners.remove(listener); }

private:
    class AlbumThumbnailComponent : public juce::Component
    {
    public:
        AlbumThumbnailComponent(const juce::String& title, const juce::Image& thumbnailImage = juce::Image())
            : albumTitle(title), thumbnail(thumbnailImage), albumId("")
        {
            if (thumbnail.isNull())
                thumbnail = juce::Image(juce::Image::RGB, 150, 150, true);
        }

        // 노래 ID 설정 (악보 파일 ID)
        void setSongId(const juce::String& id) 
        {
            songId = id;
        }

        // 노래 정보 설정
        void setSongInfo(const Song& song)
        {
            try {
                songId = song.getId();
                albumTitle = song.getTitle();
                albumArtist = song.getArtist();
                
                // 썸네일 이미지 로드 (예외 처리 추가)
                juce::File thumbnailFile(song.getThumbnailPath());
                if (thumbnailFile.existsAsFile())
                {
                    try {
                        thumbnail = juce::ImageFileFormat::loadFrom(thumbnailFile);
                        repaint();
                    }
                    catch (const std::exception& e) {
                        DBG("Error loading thumbnail image: " + thumbnailFile.getFullPathName() + 
                            " - " + juce::String(e.what()));
                        // 기본 썸네일 이미지 사용
                        thumbnail = juce::Image(juce::Image::RGB, 150, 150, true);
                        juce::Graphics g(thumbnail);
                        g.fillAll(juce::Colours::darkgrey);
                        g.setColour(juce::Colours::white);
                        g.drawText(song.getTitle(), thumbnail.getBounds().reduced(10), juce::Justification::centred, true);
                    }
                }
                else
                {
                    // 파일이 존재하지 않는 경우 기본 썸네일 사용
                    DBG("Thumbnail file does not exist: " + thumbnailFile.getFullPathName());
                    thumbnail = juce::Image(juce::Image::RGB, 150, 150, true);
                    juce::Graphics g(thumbnail);
                    g.fillAll(juce::Colours::darkgrey);
                    g.setColour(juce::Colours::white);
                    g.drawText(song.getTitle(), thumbnail.getBounds().reduced(10), juce::Justification::centred, true);
                }
            }
            catch (const std::exception& e) {
                DBG("Exception in setSongInfo: " + juce::String(e.what()));
                // 예외 발생 시 기본 상태 유지
            }
        }

        void setAlbumId(const juce::String& id) 
        {
            albumId = id;
            // 앨범 ID가 설정되면 커버 이미지 로드 시도
            loadCoverImage();
        }

        void loadCoverImage()
        {
            // SpotifyService에서 캐시된 이미지가 있는지 확인
            if (!albumCoverUrl.isEmpty())
            {
                auto cachedImage = SpotifyService::getCachedImage(albumCoverUrl);
                if (cachedImage != nullptr)
                {
                    thumbnail = *cachedImage;
                    repaint();
                    return;
                }

                // 캐시에 없으면 비동기로 로드
                SpotifyService::loadAlbumCoverAsync(albumCoverUrl, 
                    [this](std::shared_ptr<juce::Image> image) {
                        if (image)
                        {
                            thumbnail = *image;
                            repaint();
                        }
                    });
            }
        }

        void setAlbumInfo(const SpotifyService::Album& album)
        {
            albumId = album.id;
            albumTitle = album.name;
            albumArtist = album.artist;
            albumCoverUrl = album.coverUrl;
            
            if (album.coverImage)
            {
                thumbnail = *album.coverImage;
                repaint();
            }
            else
            {
                loadCoverImage();
            }
        }

        juce::String getSongId() const { return songId; }

        void mouseDown(const juce::MouseEvent& event) override
        {
            // 클릭 이벤트 처리
            selected = true;
            repaint();
            
            if (onClick)
                onClick();
        }

        void mouseUp(const juce::MouseEvent& event) override
        {
            selected = false;
            repaint();
        }

        void paint(juce::Graphics& g) override
        {
            // Draw thumbnail
            g.fillAll(juce::Colours::white);
            g.setColour(juce::Colours::lightgrey);
            g.drawRect(getLocalBounds(), 1);
            
            auto thumbnailBounds = getLocalBounds().withHeight(getHeight() - 30);
            g.drawImageWithin(thumbnail, thumbnailBounds.getX(), thumbnailBounds.getY(), 
                             thumbnailBounds.getWidth(), thumbnailBounds.getHeight(),
                             juce::RectanglePlacement::centred);
            
            // Draw title below thumbnail
            g.setColour(juce::Colours::black);
            g.drawText(albumTitle, 0, getHeight() - 30, getWidth(), 30, juce::Justification::centred);
        }

        std::function<void()> onClick;
        
    private:
        juce::String albumTitle;
        juce::String albumArtist;
        juce::String albumId;
        juce::String songId;  // 노래 ID (악보 파일 ID)
        juce::String albumCoverUrl;
        juce::Image thumbnail;
        bool selected = false;
    };
    
    class AlbumGridView : public juce::Component
    {
    public:
        AlbumGridView()
        {
            // Setup default properties
            numColumns = 3;
            thumbnailSize = 150;
            spacing = 10;
        }
        
        void addAlbum(const juce::String& title, const juce::Image& thumbnail = juce::Image())
        {
            auto* newItem = new AlbumThumbnailComponent(title, thumbnail);
            addAndMakeVisible(newItem);
            thumbnails.add(newItem);
            resized();
        }
        
        void addAlbumFromSpotify(const SpotifyService::Album& album)
        {
            auto* newItem = new AlbumThumbnailComponent(album.name);
            newItem->setAlbumInfo(album);
            addAndMakeVisible(newItem);
            thumbnails.add(newItem);
            resized();
        }

        // Song 데이터로 앨범 썸네일 추가
        void addSong(const Song& song, std::function<void(const juce::String&)> onSongSelected)
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

        void clear()
        {
            thumbnails.clear();
        }
        
        void setNumColumns(int columns)
        {
            numColumns = jmax(1, columns);
            resized();
        }
        
        void setThumbnailSize(int size)
        {
            thumbnailSize = jmax(50, size);
            resized();
        }
        
        void setSpacing(int newSpacing)
        {
            spacing = jmax(0, newSpacing);
            resized();
        }
        
        void resized() override
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
        
    private:
        juce::OwnedArray<AlbumThumbnailComponent> thumbnails;
        int numColumns;
        int thumbnailSize;
        int spacing;
    };

    juce::Label recentTitle;
    AlbumGridView recentGrid;
    juce::TextButton viewAllRecent;

    juce::Label recommendTitle;
    AlbumGridView recommendGrid;
    juce::TextButton viewAllRecommend;
    
    // Add sample data initialization method
    void initializeSampleData();
    void loadAlbumsFromCache();
    void notifySongSelected(const juce::String& songId);

    // Array 대신 ListenerList 사용
    juce::ListenerList<SongSelectedListener> songListeners;
};