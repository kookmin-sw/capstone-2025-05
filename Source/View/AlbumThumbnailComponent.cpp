#include "AlbumThumbnailComponent.h"
#include "../Controller/ContentController.h"

void AlbumThumbnailComponent::setSongInfo(const Song& song)
{
    DBG("AlbumThumbnailComponent::setSongInfo - start: " + song.getTitle() + " (ID: " + song.getId() + ")");
    
    try {
        // Song 객체 저장
        this->song = song;
        
        songId = song.getId();
        albumTitle = song.getTitle();
        albumArtist = song.getArtist();
        
        DBG("AlbumThumbnailComponent::setSongInfo - basic info set");
        
        // 이미 캐시된 이미지가 있는지 확인
        if (song.hasCachedCoverImage())
        {
            // 캐시된 이미지 사용
            DBG("AlbumThumbnailComponent::setSongInfo - using cached image");
            thumbnail = song.getCachedCoverImage();
            repaint();
            return;
        }
        
        // API에서 이미지 로드 시도
        if (contentController != nullptr && !song.getCoverImageUrl().isEmpty())
        {
            DBG("AlbumThumbnailComponent::setSongInfo - trying to load image from API");
            loadCoverImageFromAPI();
            return;
        }
        
        // 로컬 썸네일 이미지 로드 (예외 처리 추가)
        juce::String thumbnailPath = song.getThumbnailPath();
        if (!thumbnailPath.isEmpty())
        {
            // 상대 경로를 안전하게 처리
            juce::File thumbnailFile;
            
            if (thumbnailPath.startsWith("/") || thumbnailPath.startsWith("\\"))
            {
                // 절대 경로처럼 보이는 상대 경로는 작업 디렉토리 기준으로 처리
                thumbnailFile = juce::File::getCurrentWorkingDirectory().getChildFile(thumbnailPath.substring(1));
                DBG("AlbumThumbnailComponent::setSongInfo - converted path: " + thumbnailFile.getFullPathName());
            }
            else if (thumbnailPath.contains(":"))
            {
                // 드라이브 문자가 있으면 실제 절대 경로로 처리
                thumbnailFile = juce::File(thumbnailPath);
                DBG("AlbumThumbnailComponent::setSongInfo - absolute path: " + thumbnailFile.getFullPathName());
            }
            else
            {
                // 일반 상대 경로
                thumbnailFile = juce::File::getCurrentWorkingDirectory().getChildFile(thumbnailPath);
                DBG("AlbumThumbnailComponent::setSongInfo - relative path: " + thumbnailFile.getFullPathName());
            }
            
            if (thumbnailFile.existsAsFile())
            {
                try {
                    DBG("AlbumThumbnailComponent::setSongInfo - loading local thumbnail image: " + thumbnailFile.getFullPathName());
                    thumbnail = juce::ImageFileFormat::loadFrom(thumbnailFile);
                    repaint();
                }
                catch (const std::exception& e) {
                    DBG("Error loading thumbnail image: " + thumbnailFile.getFullPathName() + 
                        " - " + juce::String(e.what()));
                    // 기본 썸네일 이미지 사용
                    createDefaultThumbnail();
                }
            }
            else
            {
                // 파일이 존재하지 않는 경우 기본 썸네일 사용
                DBG("Thumbnail file does not exist: " + thumbnailFile.getFullPathName() + ", using default thumbnail");
                createDefaultThumbnail();
            }
        }
        else
        {
            // 썸네일 경로가 비어있는 경우
            DBG("AlbumThumbnailComponent::setSongInfo - empty thumbnail path, using default thumbnail");
            createDefaultThumbnail();
        }
    }
    catch (const std::exception& e) {
        DBG("Exception in setSongInfo: " + juce::String(e.what()));
        // 예외 발생 시 기본 상태 유지
    }
    
    DBG("AlbumThumbnailComponent::setSongInfo - complete");
}

void AlbumThumbnailComponent::setContentController(ContentController* controller)
{
    contentController = controller;
    
    // 컨트롤러가 설정되었고 곡 정보가 있으면 커버 이미지 로드 시도
    if (contentController != nullptr && !song.getId().isEmpty() && !song.hasCachedCoverImage())
    {
        loadCoverImageFromAPI();
    }
}

void AlbumThumbnailComponent::loadCoverImageFromAPI()
{
    if (contentController == nullptr || song.getCoverImageUrl().isEmpty())
        return;
        
    // 로딩 상태 표시
    thumbnail = juce::Image(juce::Image::ARGB, 150, 150, true);
    juce::Graphics g(thumbnail);
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::darkgrey);
    g.drawText("Loading...", thumbnail.getBounds(), juce::Justification::centred, true);
    repaint();
    
    // ContentController를 통해 커버 이미지 로드
    contentController->loadSongCoverImage(song, [this](bool success, const Song& updatedSong) {
        if (success && updatedSong.hasCachedCoverImage())
        {
            // 성공적으로 이미지를 받아온 경우
            this->song = updatedSong;
            thumbnail = updatedSong.getCachedCoverImage();
        }
        else
        {
            // 이미지 다운로드 실패 시 기본 이미지 표시
            createDefaultThumbnail();
        }
        
        repaint();
    });
}

void AlbumThumbnailComponent::createDefaultThumbnail()
{
    DBG("AlbumThumbnailComponent::createDefaultThumbnail for: " + albumTitle);
    
    // 기본 썸네일 이미지 생성
    thumbnail = juce::Image(juce::Image::RGB, 150, 150, true);
    juce::Graphics g(thumbnail);
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    
    // 앨범 타이틀 표시
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText(albumTitle, thumbnail.getBounds().reduced(10).removeFromTop(50), 
               juce::Justification::centred, true);
               
    // 아티스트 표시
    g.setFont(juce::Font(14.0f));
    g.drawText(albumArtist, thumbnail.getBounds().reduced(10).withTrimmedTop(50), 
               juce::Justification::centred, true);
    
    // 테두리 추가
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(thumbnail.getBounds().reduced(1), 2);
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