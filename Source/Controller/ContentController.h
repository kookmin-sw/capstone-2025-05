#pragma once
#include <JuceHeader.h>
#include "../Model/ContentModel.h"
#include "../View/ContentPanelComponent.h"  // ContentPanelComponent::SongSelectedListener 정의가 포함된 헤더 추가

class MainComponent;
class GuitarPracticeComponent;

// ContentController 클래스 - 컨텐츠 관련 비즈니스 로직을 처리합니다
class ContentController : public ContentPanelComponent::SongSelectedListener
{
public:
    ContentController(ContentModel& model, MainComponent& mainComponent, 
                      GuitarPracticeComponent& guitarPracticeComponent);
    ~ContentController() = default;
    
    // 곡 데이터 초기화 (로컬 샘플)
    void initializeData();
    
    // API에서 곡 데이터 로드
    void loadSongsFromAPI(std::function<void(bool success)> callback = nullptr);
    
    // 이미지 다운로드 및 캐싱
    void loadSongCoverImage(const Song& song, std::function<void(bool success, const Song& song)> callback);
    
    // 오디오 파일 다운로드
    void downloadSongAudio(const Song& song, std::function<void(bool success, const juce::String& filePath)> callback);
    
    // 악보 데이터 다운로드
    void downloadScoreData(const Song& song, std::function<void(bool success, const juce::String& scoreData)> callback);
    
    // 최근 곡 관련 처리
    const juce::Array<Song>& getRecentSongs() const;
    
    // 추천 곡 관련 처리
    const juce::Array<Song>& getRecommendedSongs() const;
    
    // 곡 선택 이벤트 처리 (ContentPanelComponent::SongSelectedListener 구현)
    void songSelected(const juce::String& songId) override;
    
    // 현재 선택된 곡 ID 반환
    juce::String getSelectedSongId() const { return selectedSongId; }
    
    // 특정 ID의 곡 가져오기
    Song getSongById(const juce::String& songId) const;
    
private:
    ContentModel& contentModel;
    MainComponent& mainComponent;
    GuitarPracticeComponent& guitarPracticeComponent;
    juce::String selectedSongId; // 현재 선택된 곡 ID
    
    // 파일 경로 관련 유틸리티
    juce::File getAudioCacheDirectory() const;
    juce::File getImageCacheDirectory() const;
    juce::File getScoreCacheDirectory() const;
};