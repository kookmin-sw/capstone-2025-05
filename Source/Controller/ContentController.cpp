#include "ContentController.h"
#include "../View/MainComponent.h"
#include "../View/GuitarPracticeComponent.h"
#include "../View/ContentPanelComponent.h"

// Controller가 Model 객체 참조를 가져오도록 수정
ContentController::ContentController(ContentModel& model, MainComponent& mainComp, 
                                    GuitarPracticeComponent& practiceSongComp)
    : contentModel(model), 
      mainComponent(mainComp),
      guitarPracticeComponent(practiceSongComp)
{
}

void ContentController::initializeData()
{
    contentModel.initializeSampleData();
}

void ContentController::loadSongsFromAPI(std::function<void(bool success)> callback)
{
    contentModel.loadSongsFromAPI([this, callback](bool success) {
        if (success)
        {
            DBG("ContentController: Songs loaded successfully from API");
        }
        else
        {
            DBG("ContentController: Failed to load songs from API");
        }
        
        if (callback)
            callback(success);
    });
}

void ContentController::loadSongCoverImage(const Song& song, std::function<void(bool success, const Song& song)> callback)
{
    DBG("ContentController::loadSongCoverImage - start for song: " + song.getTitle());
    
    // 이미 캐시된 이미지가 있는 경우
    if (song.hasCachedCoverImage())
    {
        DBG("ContentController::loadSongCoverImage - using cached image");
        if (callback)
            callback(true, song);
        return;
    }
    
    // 곡 ID 확인
    juce::String songId = song.getId();
    if (songId.isEmpty())
    {
        // ID가 없는 경우 기존 방식으로 이미지 URL 사용
        juce::String coverImageUrl = song.getCoverImageUrl();
        if (coverImageUrl.isEmpty())
        {
            DBG("ContentController::loadSongCoverImage - empty cover image URL and no song ID");
            if (callback)
                callback(false, song);
            return;
        }
        
        DBG("ContentController::loadSongCoverImage - loading from URL: " + coverImageUrl);
        
        // API 서비스를 통해 이미지 다운로드 (기존 방식)
        contentModel.getAPIService().downloadImage(coverImageUrl, [this, song, callback](bool success, juce::Image image) {
            Song updatedSong = song;
            
            if (success && !image.isNull())
            {
                // 이미지 캐싱
                DBG("ContentController::loadSongCoverImage - image downloaded successfully");
                updatedSong.setCachedCoverImage(image);
                
                // 성공 콜백 호출
                if (callback)
                    callback(true, updatedSong);
            }
            else
            {
                // 실패 처리
                DBG("ContentController::loadSongCoverImage - failed to download image");
                if (callback)
                    callback(false, song);
            }
        });
    }
    else
    {
        // 곡 ID가 있는 경우 썸네일 API 사용
        DBG("ContentController::loadSongCoverImage - loading thumbnail for song ID: " + songId);
        
        // 전용 썸네일 다운로드 메서드 사용
        contentModel.getAPIService().downloadSongThumbnail(songId, [this, song, callback](bool success, juce::Image image) {
            Song updatedSong = song;
            
            if (success && !image.isNull())
            {
                // 이미지 캐싱
                DBG("ContentController::loadSongCoverImage - thumbnail downloaded successfully");
                updatedSong.setCachedCoverImage(image);
                
                // 성공 콜백 호출
                if (callback)
                    callback(true, updatedSong);
            }
            else
            {
                // 실패 처리
                DBG("ContentController::loadSongCoverImage - failed to download thumbnail");
                if (callback)
                    callback(false, song);
            }
        });
    }
}

void ContentController::downloadSongAudio(const Song& song, std::function<void(bool success, const juce::String& filePath)> callback)
{
    DBG("ContentController::downloadSongAudio - start for song: " + song.getTitle());
    
    // 곡 ID 확인
    juce::String songId = song.getId();
    if (songId.isEmpty())
    {
        DBG("ContentController::downloadSongAudio - empty song ID");
        if (callback)
            callback(false, "No song ID provided");
        return;
    }
    
    // 오디오 URL 확인
    juce::String audioUrl = song.getAudioUrl();
    if (audioUrl.isEmpty())
    {
        DBG("ContentController::downloadSongAudio - empty audio URL");
        if (callback)
            callback(false, "No audio URL provided");
        return;
    }
    
    // API 엔드포인트 생성 - ID 기반 URL 처리
    juce::String endpoint;
    if (audioUrl.startsWith("http"))
        endpoint = audioUrl;
    else if (audioUrl.startsWith("/"))
        endpoint = audioUrl;
    else
        endpoint = "/songs/" + songId + "/audio";
    
    DBG("ContentController::downloadSongAudio - using endpoint: " + endpoint);
    
    // 캐시 파일 경로 생성 - 서버에서 제공된 파일명을 캐시 디렉토리에 저장하도록 함
    juce::File cacheDir = getAudioCacheDirectory();
    juce::File tempFile = cacheDir.getChildFile(songId + "_audio.wav");
    
    // API 서비스를 통해 오디오 파일 다운로드
    // 서버에서 제공하는 파일명을 사용할 수 있도록 함
    contentModel.getAPIService().downloadAudioFile(endpoint, tempFile, 
        [this, song, tempFile, callback, cacheDir](bool success, const juce::String& filePath) {
            if (success)
            {
                // 다운로드 성공
                DBG("ContentController::downloadSongAudio - downloaded successfully to: " + filePath);
                
                // 파일이 존재하는지 확인
                juce::File downloadedFile(filePath);
                if (!downloadedFile.existsAsFile())
                {
                    DBG("ContentController::downloadSongAudio - error: downloaded file doesn't exist");
                    if (callback)
                        callback(false, "Downloaded file doesn't exist");
                    return;
                }
                
                // 성공적으로 다운로드된 파일 경로를 사용
                if (callback)
                    callback(true, filePath);
            }
            else
            {
                // 다운로드 실패
                DBG("ContentController::downloadSongAudio - download failed");
                if (callback)
                    callback(false, "Failed to download audio file");
            }
        });
}

void ContentController::downloadScoreData(const Song& song, std::function<void(bool success, const juce::String& scoreData)> callback)
{
    // 악보 URL 확인
    juce::String scoreUrl = song.getScoreDataUrl();
    if (scoreUrl.isEmpty())
    {
        if (callback)
            callback(false, "No score data URL provided");
        return;
    }
    
    // API 서비스를 통해 악보 데이터 다운로드
    contentModel.getAPIService().downloadScoreData(scoreUrl, callback);
}

const juce::Array<Song>& ContentController::getRecentSongs() const
{
    return contentModel.getRecentSongs();
}

const juce::Array<Song>& ContentController::getRecommendedSongs() const
{
    return contentModel.getRecommendedSongs();
}

// songSelected 메서드 구현 - MainComponent에서 이동
void ContentController::songSelected(const juce::String& songId)
{
    DBG("ContentController: Song selected with ID: " + songId);
    
    // 선택된 곡 ID 저장
    selectedSongId = songId;
    
    // 곡 정보 가져오기
    Song song = getSongById(songId);
    
    // 곡이 존재하지 않는 경우
    if (song.getId().isEmpty())
    {
        DBG("ContentController: Song not found with ID: " + songId);
        return;
    }
    
    // 곡 선택 후 연습 화면으로 전환
    mainComponent.showPracticeScreen();
    
    // 곡 데이터 다운로드 후 로드
    downloadScoreData(song, [this, song](bool success, const juce::String& scoreData) {
        if (success)
        {
            // 악보 데이터 로드
            guitarPracticeComponent.loadSong(song.getId());
        }
        else
        {
            DBG("ContentController: Failed to download score data for song: " + song.getTitle());
        }
    });
}

Song ContentController::getSongById(const juce::String& songId) const
{
    return contentModel.getSongById(songId);
}

juce::File ContentController::getAudioCacheDirectory() const
{
    juce::File cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                        .getChildFile("MapleClientDesktop/cache/audio");
                        
    if (!cacheDir.exists())
        cacheDir.createDirectory();
        
    return cacheDir;
}

juce::File ContentController::getImageCacheDirectory() const
{
    juce::File cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                        .getChildFile("MapleClientDesktop/cache/images");
                        
    if (!cacheDir.exists())
        cacheDir.createDirectory();
        
    return cacheDir;
}

juce::File ContentController::getScoreCacheDirectory() const
{
    juce::File cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                        .getChildFile("MapleClientDesktop/cache/sheet_music");
                        
    if (!cacheDir.exists())
        cacheDir.createDirectory();
        
    return cacheDir;
}