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
    
    // 이미지 URL 확인
    juce::String coverImageUrl = song.getCoverImageUrl();
    if (coverImageUrl.isEmpty())
    {
        DBG("ContentController::loadSongCoverImage - empty cover image URL");
        if (callback)
            callback(false, song);
        return;
    }
    
    DBG("ContentController::loadSongCoverImage - loading from URL: " + coverImageUrl);
    
    // API 서비스를 통해 이미지 다운로드
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

void ContentController::downloadSongAudio(const Song& song, std::function<void(bool success, const juce::String& filePath)> callback)
{
    DBG("ContentController::downloadSongAudio - start for song: " + song.getTitle());
    
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
        endpoint = "/songs/" + song.getId() + "/audio";
    
    DBG("ContentController::downloadSongAudio - using endpoint: " + endpoint);
    
    // 캐시 파일 경로 생성 - 서버에서 제공된 파일명을 캐시 디렉토리에 저장하도록 함
    juce::File cacheDir = getAudioCacheDirectory();
    juce::File tempFile = cacheDir.getChildFile(song.getId() + "_temp.tmp");
    
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
            DBG("ContentController: Score data downloaded successfully for song: " + song.getTitle());
            
            // 항상 gp5 파일로 저장
            juce::File scoreCache = getScoreCacheDirectory();
            juce::File scoreFile = scoreCache.getChildFile(song.getId() + ".gp5");
            
            // 이전 파일 삭제 (있는 경우)
            if (scoreFile.existsAsFile())
            {
                DBG("ContentController: Removing existing score file");
                scoreFile.deleteFile();
            }
            
            bool fileSaved = false;
            
            // Base64 인코딩된 데이터 처리 - 모든 데이터를 Guitar Pro 파일로 간주
            if (scoreData.startsWith("data:"))
            {
                // Base64 부분만 추출
                int commaPos = scoreData.indexOf(",");
                if (commaPos != -1)
                {
                    juce::String base64Data = scoreData.substring(commaPos + 1);
                    DBG("ContentController: Base64 data length: " + juce::String(base64Data.length()));
                    
                    // Base64 디코딩
                    juce::MemoryBlock binaryData;
                    if (binaryData.fromBase64Encoding(base64Data))
                    {
                        DBG("ContentController: Successfully decoded Base64 data, size: " + 
                            juce::String(binaryData.getSize()) + " bytes");
                        
                        // GP5 헤더 검증
                        bool isValidGP5 = false;
                        if (binaryData.getSize() > 10)
                        {
                            const char* data = static_cast<const char*>(binaryData.getData());
                            // "FICHIER " 검사
                            if (data[0] == 'F' && data[1] == 'I' && data[2] == 'C' && 
                                data[3] == 'H' && data[4] == 'I' && data[5] == 'E' && 
                                data[6] == 'R' && data[7] == ' ')
                            {
                                isValidGP5 = true;
                                DBG("ContentController: Valid GP5 header found");
                            }
                            else
                            {
                                // 헤더의 첫 바이트 로깅
                                juce::String hexDump;
                                for (int i = 0; i < juce::jmin(20, static_cast<int>(binaryData.getSize())); ++i)
                                {
                                    unsigned char byte = static_cast<unsigned char>(data[i]);
                                    hexDump += juce::String::formatted("%02X ", byte);
                                }
                                DBG("ContentController: Invalid GP5 header: " + hexDump);
                            }
                        }
                        
                        // 파일로 저장
                        juce::FileOutputStream outStream(scoreFile);
                        if (outStream.openedOk())
                        {
                            outStream.write(binaryData.getData(), binaryData.getSize());
                            outStream.flush();
                            
                            // 파일 크기 확인
                            fileSaved = scoreFile.existsAsFile() && scoreFile.getSize() > 0;
                            
                            if (fileSaved)
                            {
                                DBG("ContentController: Score data saved to file: " + 
                                    scoreFile.getFullPathName() + ", size: " + 
                                    juce::String(scoreFile.getSize()) + " bytes");
                                
                                // 파일이 GP5 형식인지 최종 확인
                                juce::FileInputStream fileStream(scoreFile);
                                if (fileStream.openedOk())
                                {
                                    char header[30];
                                    fileStream.read(header, 30);
                                    
                                    // 헤더 로깅
                                    juce::String headerHex;
                                    for (int i = 0; i < 10; ++i)
                                    {
                                        unsigned char byte = static_cast<unsigned char>(header[i]);
                                        headerHex += juce::String::formatted("%02X ", byte);
                                    }
                                    DBG("ContentController: Saved file header: " + headerHex);
                                    
                                    // 헤더가 FICHIER GUITAR PRO로 시작하는지 확인하고, 버전 수정
                                    if (memcmp(header, "FICHIER GUITAR PRO", 18) == 0)
                                    {
                                        // 파일 버전 수정하기 (v5.10 -> v5.00)
                                        fileStream.setPosition(0);
                                        juce::MemoryBlock entireFile;
                                        fileStream.readIntoMemoryBlock(entireFile);
                                        
                                        // 메모리 블록 데이터 가져오기
                                        char* fileData = static_cast<char*>(entireFile.getData());
                                        size_t fileSize = entireFile.getSize();
                                        
                                        // 버전 문자열 수정
                                        bool versionModified = false;
                                        for (size_t i = 0; i < fileSize - 10; ++i)
                                        {
                                            // "v5.10" 검색
                                            if (i + 4 < fileSize && fileData[i] == 'v' && fileData[i+1] == '5' && 
                                                fileData[i+2] == '.' && fileData[i+3] == '1' && 
                                                fileData[i+4] == '0')
                                            {
                                                DBG("ContentController: Found version string at offset " + juce::String(i));
                                                // v5.10 -> v5.00 로 수정
                                                fileData[i+3] = '0';
                                                fileData[i+4] = '0';
                                                versionModified = true;
                                                DBG("ContentController: Modified version string to v5.00");
                                            }
                                        }
                                                
                                                // 수정된 데이터로 파일 덮어쓰기
                                                if (versionModified)
                                                {
                                                    juce::FileOutputStream outStream2(scoreFile);
                                                    if (outStream2.openedOk())
                                                    {
                                                        outStream2.write(entireFile.getData(), entireFile.getSize());
                                                        outStream2.flush();
                                                        DBG("ContentController: Fixed GP5 file version for better compatibility");
                                                        
                                                        // 파일 내용 확인
                                                        juce::FileInputStream verifyStream(scoreFile);
                                                        if (verifyStream.openedOk())
                                                        {
                                                            char verifyHeader[30];
                                                            verifyStream.read(verifyHeader, 30);
                                                            
                                                            // 버전 문자열 확인
                                                            for (int i = 0; i < 30; ++i)
                                                            {
                                                                if (i + 4 < 30 && verifyHeader[i] == 'v' && verifyHeader[i+1] == '5')
                                                                {
                                                                    juce::String versionStr;
                                                                    for (int j = 0; j < 5 && i+j < 30; ++j)
                                                                        versionStr += verifyHeader[i+j];
                                                                    
                                                                    DBG("ContentController: Verified version string: " + versionStr);
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                }
                            }
                            else
                            {
                                DBG("ContentController: Failed to save file or file is empty");
                            }
                        }
                        else
                        {
                            DBG("ContentController: Failed to create output stream for score file");
                        }
                    }
                    else
                    {
                        DBG("ContentController: Failed to decode Base64 data");
                    }
                }
                else
                {
                    DBG("ContentController: No comma found in Base64 encoded data");
                }
            }
            else
            {
                // 직접 데이터가 전달된 경우 (이전 버전 호환성 유지)
                DBG("ContentController: Direct data - treating as binary Guitar Pro file, length: " + 
                    juce::String(scoreData.length()));
                
                juce::FileOutputStream outStream(scoreFile);
                if (outStream.openedOk())
                {
                    // 바이너리 데이터로 저장
                    juce::MemoryBlock memoryBlock(scoreData.toRawUTF8(), scoreData.getNumBytesAsUTF8());
                    outStream.write(memoryBlock.getData(), memoryBlock.getSize());
                    outStream.flush();
                    
                    // 파일 크기 확인
                    fileSaved = scoreFile.existsAsFile() && scoreFile.getSize() > 0;
                    
                    if (fileSaved)
                    {
                        DBG("ContentController: Score data saved to file: " + 
                            scoreFile.getFullPathName() + ", size: " + 
                            juce::String(scoreFile.getSize()) + " bytes");
                    }
                    else
                    {
                        DBG("ContentController: Failed to save file or file is empty");
                    }
                }
                else
                {
                    DBG("ContentController: Failed to create output stream for score file");
                }
            }
            
            // 악보 데이터 로드
            if (fileSaved)
            {
                DBG("ContentController: Loading Guitar Pro file into GuitarPracticeComponent: " + song.getId());
                guitarPracticeComponent.loadSong(song.getId());
            }
            else
            {
                // 오류가 있는 경우 사용자에게 알림
                juce::MessageManager::callAsync([this]() {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Sheet Music Error",
                        "Unable to save sheet music file. Please check if you have sufficient disk space and permissions.",
                        "OK"
                    );
                });
                // 악보 데이터가 없으면 연습 화면을 로드하지 않음
            }
        }
        else
        {
            DBG("ContentController: Failed to download score data for song: " + song.getTitle());
            // 오류가 있는 경우 사용자에게 알림
            juce::MessageManager::callAsync([this]() {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Sheet Music Error",
                    "Unable to download sheet music. Please try again later.",
                    "OK"
                );
            });
            // 악보 데이터가 없으면 연습 화면을 로드하지 않음
        }
    });
    
    // 오디오 파일 다운로드 (병렬로 처리)
    downloadSongAudio(song, [this, song](bool success, const juce::String& filePath) {
        if (success)
        {
            DBG("ContentController: Audio file downloaded successfully for song: " + song.getTitle());
            DBG("ContentController: Audio file path: " + filePath);
            // 추가 처리 (필요한 경우)
        }
        else
        {
            DBG("ContentController: Failed to download audio file for song: " + song.getTitle());
            // 오류 처리 (필요한 경우)
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
                        .getChildFile("MapleClient")
                        .getChildFile("AudioCache");
                        
    if (!cacheDir.exists())
        cacheDir.createDirectory();
        
    return cacheDir;
}

juce::File ContentController::getImageCacheDirectory() const
{
    juce::File cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                        .getChildFile("MapleClient")
                        .getChildFile("ImageCache");
                        
    if (!cacheDir.exists())
        cacheDir.createDirectory();
        
    return cacheDir;
}

juce::File ContentController::getScoreCacheDirectory() const
{
    juce::File cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                        .getChildFile("MapleClient")
                        .getChildFile("ScoreCache");
                        
    if (!cacheDir.exists())
        cacheDir.createDirectory();
        
    return cacheDir;
}