#pragma once
#include <JuceHeader.h>
#include "../Model/Song.h"
#include "../Model/SpotifyService.h"

class ContentController;

// 앨범 썸네일을 표시하는 컴포넌트
class AlbumThumbnailComponent : public juce::Component
{
public:
    AlbumThumbnailComponent(const juce::String& title, const juce::Image& thumbnailImage = juce::Image())
        : albumTitle(title), thumbnail(thumbnailImage), albumId("")
    {
        if (thumbnail.isNull())
            thumbnail = juce::Image(juce::Image::RGB, 200, 200, true);
    }

    // 노래 ID 설정 (악보 파일 ID)
    void setSongId(const juce::String& id) 
    {
        songId = id;
    }

    // 노래 정보 설정
    void setSongInfo(const Song& song);
    
    // ContentController 설정
    void setContentController(ContentController* controller);
    
    // API에서 커버 이미지 로드
    void loadCoverImageFromAPI();

    void setAlbumId(const juce::String& id) 
    {
        albumId = id;
        // 앨범 ID가 설정되면 커버 이미지 로드 시도
        loadCoverImage();
    }

    void loadCoverImage();

    void setAlbumInfo(const SpotifyService::Album& album);

    juce::String getSongId() const { return songId; }

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void paint(juce::Graphics& g) override;

    std::function<void()> onClick;
    
private:
    juce::String albumTitle;
    juce::String albumArtist;
    juce::String albumId;
    juce::String songId;  // 노래 ID (악보 파일 ID)
    juce::String albumCoverUrl;
    juce::Image thumbnail;
    bool selected = false;
    
    Song song; // 연관된 Song 객체를 보관
    ContentController* contentController = nullptr;
    
    // 기본 썸네일 이미지 생성
    void createDefaultThumbnail();
};