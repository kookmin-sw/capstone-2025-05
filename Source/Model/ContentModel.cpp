#include "ContentModel.h"

ContentModel::ContentModel()
{
}

void ContentModel::addRecentSong(const Song& song)
{
    recentSongs.add(song);
}

const juce::Array<Song>& ContentModel::getRecentSongs() const
{
    return recentSongs;
}

void ContentModel::clearRecentSongs()
{
    recentSongs.clear();
}

void ContentModel::addRecommendedSong(const Song& song)
{
    recommendedSongs.add(song);
}

const juce::Array<Song>& ContentModel::getRecommendedSongs() const
{
    return recommendedSongs;
}

void ContentModel::clearRecommendedSongs()
{
    recommendedSongs.clear();
}

void ContentModel::initializeSampleData()
{
    // 기존 데이터 초기화
    clearRecentSongs();
    clearRecommendedSongs();
    
    // 샘플 악보 데이터 추가
    Song song1("song1", "Homecoming", "Green Day", "D:/audio_dataset/thumbnails/song1.jpg");
    Song song2("song2", "Sweet Child O' Mine", "Guns N' Roses", "D:/audio_dataset/thumbnails/song2.jpg");
    Song song3("song3", "Wonderwall", "Oasis", "D:/audio_dataset/thumbnails/song3.jpg");
    
    // 연습 가능한 곡 추가
    addRecentSong(song1);
    addRecentSong(song2);
    addRecentSong(song3);
    
    // 추천 곡 섹션
    Song song4("song4", "Nothing Else Matters", "Metallica", "D:/audio_dataset/thumbnails/song4.jpg");
    Song song5("song5", "Hotel California", "Eagles", "D:/audio_dataset/thumbnails/song5.jpg");
    
    addRecommendedSong(song4);
    addRecommendedSong(song5);
}