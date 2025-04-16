#include "AlbumThumbnailComponent.h"

void AlbumThumbnailComponent::setSongInfo(const Song& song)
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

void AlbumThumbnailComponent::loadCoverImage()
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

void AlbumThumbnailComponent::setAlbumInfo(const SpotifyService::Album& album)
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

void AlbumThumbnailComponent::mouseDown(const juce::MouseEvent& event)
{
    // 클릭 이벤트 처리
    selected = true;
    repaint();
    
    if (onClick)
        onClick();
}

void AlbumThumbnailComponent::mouseUp(const juce::MouseEvent& event)
{
    selected = false;
    repaint();
}

void AlbumThumbnailComponent::paint(juce::Graphics& g)
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