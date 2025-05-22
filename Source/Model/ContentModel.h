#pragma once
#include <JuceHeader.h>
#include "Song.h"
#include "../API/SongsAPIService.h"

// 콘텐츠 모델 클래스 - 앨범 및 추천 곡 정보를 관리
class ContentModel
{
public:
    ContentModel();
    ~ContentModel() = default;
    
    // 최근/재생 가능한 곡 관리
    void addRecentSong(const Song& song);
    const juce::Array<Song>& getRecentSongs() const;
    void clearRecentSongs();
    
    // 추천 곡 관리
    void addRecommendedSong(const Song& song);
    const juce::Array<Song>& getRecommendedSongs() const;
    void clearRecommendedSongs();
    
    // 샘플 데이터 초기화 (개발용)
    void initializeSampleData();
    
    // API에서 데이터 로드
    void loadSongsFromAPI(std::function<void(bool success)> callback);
    
    // 특정 ID의 곡 찾기
    Song getSongById(const juce::String& songId) const;
    
    // API 서비스 접근자
    SongsAPIService& getAPIService() { return apiService; }
    
private:
    juce::Array<Song> recentSongs;
    juce::Array<Song> recommendedSongs;
    SongsAPIService apiService;
    
    // 곡 ID를 기준으로 고유한 곡 컬렉션 유지
    juce::HashMap<juce::String, int> songIdToIndex;
};