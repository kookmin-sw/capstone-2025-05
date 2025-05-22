#pragma once
#include <JuceHeader.h>

// 노래(악보) 데이터 모델
class Song
{
public:
    // 기본 생성자 추가
    Song() = default;
    
    // 기존 생성자
    Song(const juce::String& id, const juce::String& title, const juce::String& artist, const juce::String& thumbnailPath)
        : id(id), title(title), artist(artist), thumbnailPath(thumbnailPath)
    {
    }
    
    // 확장된 생성자
    Song(const juce::String& id, const juce::String& title, const juce::String& artist, 
         const juce::String& thumbnailPath, const juce::String& coverImageUrl, 
         const juce::String& audioUrl, const juce::String& scoreDataUrl, 
         const juce::String& genre, const juce::String& albumName)
        : id(id), title(title), artist(artist), thumbnailPath(thumbnailPath),
          coverImageUrl(coverImageUrl), audioUrl(audioUrl), scoreDataUrl(scoreDataUrl),
          genre(genre), albumName(albumName)
    {
    }
    
    // 기존 게터
    juce::String getId() const { return id; }
    juce::String getTitle() const { return title; }
    juce::String getArtist() const { return artist; }
    juce::String getThumbnailPath() const { return thumbnailPath; }
    
    // 추가된 게터
    juce::String getCoverImageUrl() const { return coverImageUrl; }
    juce::String getAudioUrl() const { return audioUrl; }
    juce::String getScoreDataUrl() const { return scoreDataUrl; }
    juce::String getGenre() const { return genre; }
    juce::String getAlbumName() const { return albumName; }
    
    // 캐시된 이미지 관리
    void setCachedCoverImage(const juce::Image& image) { cachedCoverImage = image; }
    juce::Image getCachedCoverImage() const { return cachedCoverImage; }
    bool hasCachedCoverImage() const { return !cachedCoverImage.isNull(); }
    
    // JSON에서 Song 객체 생성
    static Song fromJson(const juce::var& json)
    {
        DBG("Song::fromJson - start");
        
        if (!json.isObject())
        {
            DBG("Song::fromJson - input is not a valid JSON object");
            return Song();
        }
            
        Song song;
        song.id = json.getProperty("song_id", "").toString();
        song.title = json.getProperty("title", "").toString();
        song.artist = json.getProperty("artist", "").toString();
        
        // 썸네일/커버 이미지 URL
        juce::String thumbnailPath = json.getProperty("thumbnail", "").toString();
        song.thumbnailPath = thumbnailPath;
        
        // 커버 이미지 URL 설정 - ID가 있는 경우 API 엔드포인트 사용 (apiBaseUrl이 이미 /api/v1 포함)
        if (!song.id.isEmpty())
        {
            // ID 기반 썸네일 API 엔드포인트 설정
            song.coverImageUrl = "/songs/" + song.id + "/thumbnail";
            DBG("Song::fromJson - generated thumbnail URL from ID: " + song.coverImageUrl);
        }
        else
        {
            song.coverImageUrl = thumbnailPath;
            DBG("Song::fromJson - using provided thumbnail path: " + thumbnailPath);
        }
        
        // 오디오 경로 - song_id 기반 API 엔드포인트 설정
        juce::String audioPath = json.getProperty("audio", "").toString();
        if (audioPath.isEmpty() && !song.id.isEmpty())
        {
            // ID 기반 오디오 경로 설정
            song.audioUrl = "/songs/" + song.id + "/audio";
            DBG("Song::fromJson - generated audio URL from ID: " + song.audioUrl);
        }
        else
        {
            song.audioUrl = audioPath;
            DBG("Song::fromJson - using provided audio URL: " + audioPath);
        }
        
        // 악보 데이터 URL
        song.scoreDataUrl = json.getProperty("sheet_music", "").toString();
        
        song.genre = json.getProperty("genre", "").toString();
        song.albumName = json.getProperty("album", "").toString();
        
        DBG("Song::fromJson - created song: " + song.id + " - " + song.title);
        DBG("Song::fromJson - thumbnail: " + song.thumbnailPath);
        DBG("Song::fromJson - audio: " + song.audioUrl);
        DBG("Song::fromJson - score: " + song.scoreDataUrl);
        return song;
    }
    
    // Song 객체 배열 생성
    static juce::Array<Song> fromJsonArray(const juce::var& jsonArray)
    {
        DBG("Song::fromJsonArray - start");
        juce::Array<Song> songs;
        
        if (jsonArray.isArray())
        {
            int numSongs = jsonArray.getArray()->size();
            DBG("Song::fromJsonArray - processing array with " + juce::String(numSongs) + " items");
            
            for (int i = 0; i < numSongs; ++i)
            {
                auto songJson = jsonArray[i];
                DBG("Song::fromJsonArray - processing item " + juce::String(i));
                songs.add(fromJson(songJson));
            }
        }
        else
        {
            DBG("Song::fromJsonArray - input is not a valid JSON array");
        }
        
        DBG("Song::fromJsonArray - completed, created " + juce::String(songs.size()) + " songs");
        return songs;
    }
    
private:
    juce::String id;
    juce::String title;
    juce::String artist;
    juce::String thumbnailPath;
    
    // API에서 받아온 추가 데이터
    juce::String coverImageUrl;
    juce::String audioUrl;
    juce::String scoreDataUrl;
    juce::String genre;
    juce::String albumName;
    
    // 캐시된 커버 이미지
    juce::Image cachedCoverImage;
};