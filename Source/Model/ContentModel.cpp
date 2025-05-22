#include "ContentModel.h"

ContentModel::ContentModel()
{
    DBG("ContentModel constructor");
}

void ContentModel::addRecentSong(const Song& song)
{
    // 동일한 ID의 곡이 이미 있는지 확인
    for (int i = 0; i < recentSongs.size(); ++i)
    {
        if (recentSongs[i].getId() == song.getId())
        {
            // 이미 있는 경우 업데이트
            recentSongs.remove(i);
            break;
        }
    }
    
    // 새 곡 추가
    recentSongs.add(song);
    DBG("Recent song added: " + song.getTitle() + " (ID: " + song.getId() + "), total: " + juce::String(recentSongs.size()));
}

const juce::Array<Song>& ContentModel::getRecentSongs() const
{
    DBG("Getting recent songs, count: " + juce::String(recentSongs.size()));
    return recentSongs;
}

void ContentModel::clearRecentSongs()
{
    recentSongs.clear();
    DBG("Recent songs cleared");
}

void ContentModel::addRecommendedSong(const Song& song)
{
    // 동일한 ID의 곡이 이미 있는지 확인
    for (int i = 0; i < recommendedSongs.size(); ++i)
    {
        if (recommendedSongs[i].getId() == song.getId())
        {
            // 이미 있는 경우 업데이트
            recommendedSongs.remove(i);
            break;
        }
    }
    
    // 새 곡 추가
    recommendedSongs.add(song);
    DBG("Recommended song added: " + song.getTitle() + " (ID: " + song.getId() + "), total: " + juce::String(recommendedSongs.size()));
}

const juce::Array<Song>& ContentModel::getRecommendedSongs() const
{
    DBG("Getting recommended songs, count: " + juce::String(recommendedSongs.size()));
    return recommendedSongs;
}

void ContentModel::clearRecommendedSongs()
{
    recommendedSongs.clear();
    DBG("Recommended songs cleared");
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

void ContentModel::loadSongsFromAPI(std::function<void(bool success)> callback)
{
    // 기존 데이터 초기화
    clearRecentSongs();
    clearRecommendedSongs();
    songIdToIndex.clear();
    
    DBG("ContentModel::loadSongsFromAPI - start");
    
    // API 서비스를 통해 모든 곡 정보 가져오기
    apiService.getAllSongs([this, callback](ApiResponse response) {
        bool success = response.success;
        
        DBG("ContentModel::loadSongsFromAPI - API response received, success: " + juce::String(success ? "true" : "false"));
        
        if (success)
        {
            // 응답 데이터 확인
            juce::var songsData = response.data;
            
            // API 응답이 객체이고 songs 필드가 있는 경우 (일반적인 API 패턴)
            if (response.data.isObject() && response.data.hasProperty("songs"))
            {
                DBG("ContentModel::loadSongsFromAPI - response has 'songs' property");
                songsData = response.data.getProperty("songs", juce::var());
            }
            
            // 응답이 배열 또는 songs 필드가 배열인지 확인
            if (songsData.isArray())
            {
                DBG("ContentModel::loadSongsFromAPI - processing songs array with " + 
                    juce::String(songsData.getArray()->size()) + " items");
                
                // 응답의 첫 번째 항목 확인
                if (songsData.getArray()->size() > 0)
                {
                    auto firstItem = songsData[0];
                    if (firstItem.isObject())
                    {
                        DBG("ContentModel::loadSongsFromAPI - first song properties:");
                        
                        for (const auto& prop : firstItem.getDynamicObject()->getProperties())
                        {
                            DBG(" - " + prop.name.toString() + ": " + prop.value.toString());
                        }
                    }
                }
                
                // 응답 데이터에서 곡 배열로 변환
                juce::Array<Song> songs = Song::fromJsonArray(songsData);
                DBG("ContentModel::loadSongsFromAPI - converted to " + juce::String(songs.size()) + " Song objects");
                
                // 받아온 곡들을 카테고리별로 분류
                for (int i = 0; i < songs.size(); ++i)
                {
                    const Song& song = songs[i];
                    
                    // 홀수/짝수 인덱스에 따라 분류하여 고르게 보여주기
                    if (i % 2 == 0)
                    {
                        DBG("ContentModel::loadSongsFromAPI - adding to recent songs: " + song.getTitle());
                        addRecentSong(song);
                    }
                    else
                    {
                        DBG("ContentModel::loadSongsFromAPI - adding to recommended songs: " + song.getTitle());
                        addRecommendedSong(song);
                    }
                    
                    // ID로 빠른 검색을 위한 맵 업데이트
                    songIdToIndex.set(song.getId(), i);
                }
            }
            else
            {
                DBG("ContentModel::loadSongsFromAPI - response data is not a valid songs array");
                success = false;
            }
        }
        else
        {
            DBG("ContentModel::loadSongsFromAPI - failed with error: " + response.errorMessage);
        }
        
        // 결과 요약
        DBG("ContentModel::loadSongsFromAPI - complete, recent songs: " + juce::String(recentSongs.size()) + 
            ", recommended songs: " + juce::String(recommendedSongs.size()));
        
        // 콜백 호출
        if (callback)
            callback(success);
    });
}

Song ContentModel::getSongById(const juce::String& songId) const
{
    // 최근 곡에서 검색
    for (const auto& song : recentSongs)
    {
        if (song.getId() == songId)
            return song;
    }
    
    // 추천 곡에서 검색
    for (const auto& song : recommendedSongs)
    {
        if (song.getId() == songId)
            return song;
    }
    
    // 찾지 못한 경우 빈 곡 객체 반환
    return Song();
}