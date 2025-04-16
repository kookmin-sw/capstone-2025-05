#pragma once
#include <JuceHeader.h>
#include "Song.h"

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
    
    // 샘플 데이터 초기화
    void initializeSampleData();
    
private:
    juce::Array<Song> recentSongs;
    juce::Array<Song> recommendedSongs;
};