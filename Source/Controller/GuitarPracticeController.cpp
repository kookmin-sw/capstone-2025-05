#include "GuitarPracticeController.h"
#include "View/GuitarPracticeComponent.h"
#include "Util/EnvLoader.h"
#include "API/SongsAPIService.h"

GuitarPracticeController::GuitarPracticeController(AudioModel& model, juce::AudioDeviceManager& deviceManager)
    : audioModel(model), deviceManager(deviceManager)
{
    // 컨트롤러 초기화
    analysisTimer = std::make_unique<AnalysisTimer>(*this);
    
    // TabPlayer 초기화
    auto currentDevice = deviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr)
    {
        player.prepareToPlay(currentDevice->getCurrentSampleRate(), 
                             currentDevice->getCurrentBufferSizeSamples());
    }
    else
    {
        // 기본값 설정
        player.prepareToPlay(44100.0, 512);
    }
    
    // TabPlayer를 오디오 콜백으로 등록
    deviceManager.addAudioCallback(&player);
    
    // SongsAPIService 초기화
    apiService = std::make_unique<SongsAPIService>();
    
    // 환경 변수에서 API URL 설정
    juce::String apiUrl = EnvLoader::get("MAPLE_API_URL");
    if (apiUrl.isNotEmpty())
    {
        apiService->setApiBaseUrl(apiUrl);
    }
    #ifdef JUCE_DEBUG
    // 디버그 모드에서는 localhost 사용
    apiService->setApiBaseUrl("http://localhost:8000");
    #endif
    
    DBG("GuitarPracticeController: API URL set to: " + apiService->getApiBaseUrl());
}

GuitarPracticeController::~GuitarPracticeController()
{
    // 스레드 정리
    currentAnalysisThread = nullptr;
    
    // 타이머 정리
    if (analysisTimer)
        analysisTimer->stopTimer();
    
    // 오디오 콜백 제거
    deviceManager.removeAudioCallback(&player);
}

void GuitarPracticeController::startPlayback()
{
    // 이미 재생 중인지 확인하여 중복 호출 방지
    if (!audioModel.isPlaying())
    {
        // 모델의 재생 상태 변경
        audioModel.setPlaying(true);
        
        // TabPlayer의 실제 재생 시작
        if (!player.isPlaying())
        {
            player.startPlaying();
            DBG("TabPlayer: Playback started");
        }
    }
}

void GuitarPracticeController::stopPlayback()
{
    // 이미 정지 상태인지 확인하여 중복 호출 방지
    if (audioModel.isPlaying())
    {
        // 모델의 재생 상태 변경
        audioModel.setPlaying(false);
        
        // TabPlayer의 실제 재생 중지
        if (player.isPlaying())
        {
            player.stopPlaying();
            DBG("TabPlayer: Playback stopped");
        }
    }
}

void GuitarPracticeController::togglePlayback()
{
    // TabPlayer와 AudioModel의 상태 확인
    bool playerIsPlaying = player.isPlaying();
    bool modelIsPlaying = audioModel.isPlaying();
    
    // 상태가 불일치하면 동기화
    if (playerIsPlaying != modelIsPlaying)
    {
        DBG("State mismatch detected! Player playing: " + juce::String(playerIsPlaying ? "true" : "false") + 
            ", Model playing: " + juce::String(modelIsPlaying ? "true" : "false"));
        
        // TabPlayer 상태를 기준으로 AudioModel 동기화
        audioModel.setPlaying(playerIsPlaying);
    }
    
    // 이제 상태가 동기화되었으므로 토글
    if (audioModel.isPlaying())
    {
        stopPlayback();
    }
    else
    {
        startPlayback();
    }
}

bool GuitarPracticeController::loadSong(const juce::String& songId)
{
    DBG("GuitarPracticeController::loadSong - ID: " + songId);
    
    // 기존에 재생 중이라면 중지
    if (audioModel.isPlaying())
    {
        stopPlayback();
    }
    
    resetParser();
    
    // 최근 로드한 곡 ID 저장
    currentSongId = songId;
    
    // 서버에서 곡 정보 조회
    apiService->getSongById(songId, [this, songId](ApiResponse response) {
        if (!response.success)
        {
            DBG("Error loading song info: " + response.errorMessage);
            publishSongLoadFailedEvent(songId, "Failed to get song info from server: " + response.errorMessage);
            return;
        }
        
        // 곡 정보에서 악보 파일 URL 추출
        juce::String sheetMusicUrl = response.data.getProperty("sheet_music", "").toString();
        
        if (sheetMusicUrl.isEmpty())
        {
            DBG("Sheet music URL is empty for song ID: " + songId);
            publishSongLoadFailedEvent(songId, "Sheet music URL not found");
            return;
        }
        
        DBG("Found sheet music URL: " + sheetMusicUrl);
        
        // 캐시 디렉토리 준비
        juce::File cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                              .getChildFile("MapleClientDesktop/cache/sheet_music");
        if (!cacheDir.exists())
            cacheDir.createDirectory();
        
        // 파일 이름 추출
        juce::String fileName = sheetMusicUrl.fromLastOccurrenceOf("/", false, false);
        if (fileName.isEmpty())
            fileName = songId + ".gp5";
        
        // 캐시 파일 경로 설정
        juce::File cacheFile = cacheDir.getChildFile(fileName);
        
        DBG("Downloading sheet music file to: " + cacheFile.getFullPathName());
        
        // 악보 데이터 URL 엔드포인트 직접 구성 (선택 사항)
        juce::String scoreEndpoint = "/songs/" + songId + "/sheet";
        
        // 오디오 서비스의 downloadAudioFile 메서드 사용 - 악보 파일도 바이너리 파일이므로 이 메서드 활용
        apiService->downloadAudioFile(sheetMusicUrl, cacheFile, [this, songId](bool success, juce::String filePath) {
            if (!success)
            {
                DBG("Failed to download sheet music file");
                publishSongLoadFailedEvent(songId, "Failed to download sheet music file");
                return;
            }
            
            DBG("Sheet music file downloaded to: " + filePath);
            
            try {
                // 파서 생성 및 악보 로드 (포인터로 변경됨)
                DBG("Loading GP file from: " + filePath);
                parserPtr = std::make_unique<gp_parser::Parser>(filePath.toRawUTF8());
                
                try {
                    // TabPlayer에 악보 데이터 설정
                    // Parser로부터 TabFile 획득
                    auto tabFile = parserPtr->getTabFile();
                    
                    // 벡터 요소 유효성 검사 (빈 트랙 검사)
                    if (tabFile.tracks.empty()) {
                        DBG("Warning: Loaded TabFile has no tracks");
                        
                        // 빈 트랙이 있는 경우 기본 TabFile 생성
                        tracks.clear();
                        measureHeaders.clear();
                        channels.clear();
                        // 디폴트 값으로 빈 TabFile 생성
                        player.setTabFile(gp_parser::TabFile(major, minor, title, subtitle, artist, album,
                                                        lyricsAuthor, musicAuthor, copyright, tab, instructions,
                                                        comments, lyric, tempoValue, globalKeySignature, channels,
                                                        measures, trackCount, measureHeaders, tracks));
                    } 
                    else {
                        DBG("TabFile loaded successfully with " + juce::String(tabFile.tracks.size()) + " tracks");
                        
                        // 첫 번째 트랙에 마디가 있는지 확인
                        if (tabFile.tracks[0].measures.empty()) {
                            DBG("Warning: First track has no measures");
                        }
                        else {
                            DBG("First track has " + juce::String(tabFile.tracks[0].measures.size()) + " measures");
                        }
                        
                        // TabPlayer에 TabFile 설정
                        player.setTabFile(tabFile);
                    }
                    
                    DBG("Song loaded successfully: " + songId);
                    publishSongLoadedEvent(songId);
                }
                catch (const std::exception& e) {
                    DBG("Error in parser.getTabFile() or player.setTabFile(): " + juce::String(e.what()));
                    publishSongLoadFailedEvent(songId, "Error parsing sheet music file: " + juce::String(e.what()));
                }
            }
            catch (const std::exception& e) {
                DBG("Error loading song: " + juce::String(e.what()));
                publishSongLoadFailedEvent(songId, "Error loading sheet music file: " + juce::String(e.what()));
            }
            catch (...) {
                DBG("Unknown error loading song");
                publishSongLoadFailedEvent(songId, "Unknown error occurred");
            }
        });
    });
    
    // 비동기 작업이 진행 중임을 표시하기 위해 true 반환
    return true;
}

void GuitarPracticeController::resetParser()
{
    // 재생 중이면 중지
    if (audioModel.isPlaying()) {
        stopPlayback();
    }
}

// 녹음 관련 메서드는 View가 AudioRecorder 객체를 관리하므로 추후 구현

bool GuitarPracticeController::isRecording() const
{
    // AudioRecorder 객체가 View에 있으므로 실제 구현은 View에서 호출
    return false;
}

void GuitarPracticeController::startRecording()
{
    // 재생 중이면 중지
    if (audioModel.isPlaying())
        togglePlayback();
    
    // 실제 녹음 시작은 View에서 처리
}

void GuitarPracticeController::stopRecording()
{
    // 실제 녹음 중지는 View에서 처리
}

// 분석 스레드 클래스 선언
class GuitarPracticeController::AnalysisThread : public juce::ThreadWithProgressWindow
{
public:
    AnalysisThread(const juce::String& url, const juce::File& recording)
        : juce::ThreadWithProgressWindow("Analyzing Recording...", true, true),
          serverUrl(url),
          audioFile(recording)
    {
    }
    
    void run() override
    {
        // 프로그레스 바 업데이트
        setProgress(0.1);
        setStatusMessage("Reading audio file...");
        
        // 오디오 파일 존재 확인
        if (!audioFile.existsAsFile())
        {
            setStatusMessage("Error: Audio file not found!");
            juce::Thread::sleep(1500);
            return;
        }
        
        // 파일 데이터를 메모리 블록으로 읽기
        juce::MemoryBlock fileData;
        if (!audioFile.loadFileAsData(fileData))
        {
            setStatusMessage("Error: Could not read audio file!");
            juce::Thread::sleep(1500);
            return;
        }
        
        setProgress(0.3);
        setStatusMessage("Sending data to analysis server...");
        
        // 통신 상태 디버깅을 위한 로그 추가
        DBG("Connecting to server: " + serverUrl);
        
        try {
            // 파이썬 코드처럼 URL Parameters 사용 방식으로 변경
            juce::URL url(serverUrl);
            
            // URL 쿼리 파라미터로 설정 (파이썬의 params 처럼)
            url = url.withParameter("user_id", "desktop_user");
            url = url.withParameter("song_id", "current_song");
            url = url.withParameter("generate_feedback", "true");
            
            DBG("Full URL with parameters: " + url.toString(true));
            
            // 멀티파트 폼 데이터 구성을 간소화
            juce::StringPairArray responseHeaders;
            int statusCode = 0;
            
            // POST 요청 생성 (파일 첨부)
            url = url.withFileToUpload("file", audioFile, "audio/wav");
            
            setProgress(0.5);
            setStatusMessage("Sending audio to server...");
            
            // 요청 전송 - InputStreamOptions를 사용하고 필요한 옵션 설정
            auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                                .withResponseHeaders(&responseHeaders)
                                .withStatusCode(&statusCode)
                                .withConnectionTimeoutMs(30000);  // 30초 타임아웃
            
            std::unique_ptr<juce::InputStream> input = url.createInputStream(options);
            
            // 응답 처리
            if (input == nullptr)
            {
                DBG("Connection failed. Status code: " + juce::String(statusCode));
                DBG("Response headers: " + responseHeaders.getDescription());
                
                setStatusMessage("Error: Failed to connect to the server! Code: " + juce::String(statusCode));
                juce::Thread::sleep(1500);
                return;
            }
            
            DBG("Connection successful. Status code: " + juce::String(statusCode));
            
            // 응답 헤더 디버깅
            DBG("Response headers:");
            for (auto& key : responseHeaders.getAllKeys())
            {
                DBG("  " + key + ": " + responseHeaders[key]);
            }
            
            setProgress(0.7);
            setStatusMessage("Processing response...");
            
            // 응답 읽기
            juce::String response = input->readEntireStreamAsString();
            DBG("Response content (first 200 chars): " + response.substring(0, 200));
            
            // JSON 응답 파싱
            juce::var jsonResponse = juce::JSON::parse(response);
            
            // 작업 ID 추출
            juce::String taskId;
            if (jsonResponse.hasProperty("task_id"))
            {
                taskId = jsonResponse["task_id"].toString();
                DBG("Task ID received: " + taskId);
            }
            else
            {
                DBG("Invalid response: no task_id found. Response: " + response);
                setStatusMessage("Error: Invalid response from server!");
                juce::Thread::sleep(1500);
                return;
            }
            
            // 작업 상태 폴링
            int attempts = 0;
            const int maxAttempts = 120; // 최대 120번 시도 (120초)
            
            setProgress(0.8);
            setStatusMessage("Waiting for analysis results...");
            
            while (attempts < maxAttempts)
            {
                if (threadShouldExit())
                    return;
                    
                // 작업 상태 확인 요청
                juce::URL statusUrl(serverUrl.upToLastOccurrenceOf("/analyze", false, false) + 
                                  "/tasks/" + taskId);
                
                // InputStreamOptions 사용
                auto statusOptions = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                                    .withConnectionTimeoutMs(5000);
                std::unique_ptr<juce::InputStream> statusInput = statusUrl.createInputStream(statusOptions);
                
                if (statusInput != nullptr)
                {
                    juce::String statusResponse = statusInput->readEntireStreamAsString();
                    statusJson = juce::JSON::parse(statusResponse);
                    
                    if (statusJson.hasProperty("status"))
                    {
                        juce::String status = statusJson["status"].toString();
                        
                        // 진행률 업데이트
                        if (statusJson.hasProperty("progress"))
                        {
                            float progress = static_cast<float>(static_cast<int>(statusJson["progress"])) / 100.0f;
                            setProgress(0.8f + (progress * 0.2f));
                            setStatusMessage("Analyzing: " + juce::String(static_cast<int>(progress * 100)) + "%");
                        }
                        
                        if (status == "SUCCESS")
                        {
                            // 분석 완료
                            analysisCompleted = true;
                            return;
                        }
                        else if (status == "FAILURE")
                        {
                            // 분석 실패
                            errorMessage = "Analysis failed.";
                            if (statusJson.hasProperty("error"))
                            {
                                errorMessage = statusJson["error"].toString();
                            }
                            return;
                        }
                    }
                }
                
                attempts++;
                wait(1000); // 1초 대기
            }
            
            // 타임아웃
            errorMessage = "The analysis is taking longer than expected. Please check the results later.";
        }
        catch (const std::exception& e)
        {
            DBG("Exception during server communication: " + juce::String(e.what()));
            setStatusMessage("Error: Exception occurred during server communication!");
            juce::Thread::sleep(1500);
        }
    }
    
    juce::String serverUrl;
    juce::File audioFile;
    juce::var statusJson;
    bool analysisCompleted = false;
    juce::String errorMessage;
};

void GuitarPracticeController::analyzeRecording(const juce::File& recordingFile)
{
    if (!recordingFile.existsAsFile())
    {
        DBG("No recorded file to analyze.");
        
        if (view)
        {
            juce::MessageManager::callAsync([this]() {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Analysis Error",
                    "No recording file found. Please record audio first.",
                    "OK"
                );
            });
        }
        return;
    }
    
    DBG("[ANALYSIS] Starting analysis with file: " + recordingFile.getFullPathName());
    
    juce::String serverUrl = EnvLoader::get("MAPLE_ANALYSIS_API_URL") + "analyze";
    
    #ifdef JUCE_DEBUG
    serverUrl = "http://localhost:8000/api/v1/analyze";
    #endif
    
    // Clean up existing thread
    currentAnalysisThread = nullptr;
    
    // Create new analysis thread
    currentAnalysisThread = std::make_unique<AnalysisThread>(serverUrl, recordingFile);
    
    // Set progress window message
    currentAnalysisThread->setStatusMessage("Preparing analysis...");
    
    // Start thread (asynchronously)
    currentAnalysisThread->launchThread();
    
    // Debug info
    DBG("[ANALYSIS] Thread launched");
    
    // Start timer
    analysisTimer->startTimer(500);
}

void GuitarPracticeController::handleAnalysisThreadComplete()
{
    if (!currentAnalysisThread)
        return;
        
    DBG("[ANALYSIS] Processing thread completion");
    
    if (currentAnalysisThread->threadShouldExit())
    {
        DBG("[ANALYSIS] Thread was cancelled");
        publishAnalysisFailedEvent("Analysis was cancelled by the user");
    }
    else if (currentAnalysisThread->analysisCompleted)
    {
        DBG("[ANALYSIS] Thread completed successfully");
        
        if (currentAnalysisThread->statusJson.isVoid())
        {
            DBG("[ANALYSIS] ERROR: statusJson is void!");
            publishAnalysisFailedEvent("The analysis completed but no data was returned");
        }
        else
        {
            DBG("[ANALYSIS] Successful analysis detected");
            
            // Publish event: Send analysis results through EventBus
            publishAnalysisCompleteEvent(currentAnalysisThread->statusJson);
            
            // Legacy approach - direct UI update (temporarily keeping both methods)
            if (view)
            {
                juce::MessageManager::callAsync([this]() {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::InfoIcon,
                        "Analysis Complete",
                        "The analysis has been completed successfully.",
                        "OK"
                    );
                });
            }
        }
    }
    else if (currentAnalysisThread->errorMessage.isNotEmpty())
    {
        DBG("[ANALYSIS] Thread failed: " + currentAnalysisThread->errorMessage);
        
        // Publish event: Send analysis failure through EventBus
        publishAnalysisFailedEvent(currentAnalysisThread->errorMessage);
        
        // Legacy approach - direct UI update (temporarily keeping both methods)
        if (view)
        {
            juce::String errorMsg = currentAnalysisThread->errorMessage;
            
            juce::MessageManager::callAsync([this, errorMsg]() {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Analysis Failed",
                    errorMsg,
                    "OK"
                );
            });
        }
    }
    else
    {
        DBG("[ANALYSIS] Thread finished with unknown status");
        publishAnalysisFailedEvent("The analysis process completed, but the result is unknown");
        
        // Legacy approach - direct UI update (temporarily keeping both methods)
        if (view)
        {
            juce::MessageManager::callAsync([this]() {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Analysis Status",
                    "The analysis process completed, but the result is unknown.",
                    "OK"
                );
            });
        }
    }
    
    // Clean up thread object
    currentAnalysisThread = nullptr;
}

// AnalysisTimer callback implementation
void GuitarPracticeController::AnalysisTimer::timerCallback()
{
    // Stop timer if no analysis thread exists
    if (!controller.currentAnalysisThread)
    {
        stopTimer();
        return;
    }
    
    // If thread is not running (completed or cancelled)
    if (!controller.currentAnalysisThread->isThreadRunning())
    {
        DBG("[ANALYSIS] Thread is no longer running - checking results");
        controller.handleAnalysisThreadComplete();
        stopTimer();
    }
}

void GuitarPracticeController::publishAnalysisCompleteEvent(const juce::var& result)
{
    // Create and publish analysis complete event
    auto event = std::make_shared<AnalysisCompleteEvent>(result);
    EventBus::getInstance().publishOnMainThread(event);
    
    DBG("GuitarPracticeController: Published AnalysisCompleteEvent");
}

void GuitarPracticeController::publishAnalysisFailedEvent(const juce::String& errorMessage)
{
    // Create and publish analysis failed event
    auto event = std::make_shared<AnalysisFailedEvent>(errorMessage);
    EventBus::getInstance().publishOnMainThread(event);
    
    DBG("GuitarPracticeController: Published AnalysisFailedEvent: " + errorMessage);
}

void GuitarPracticeController::publishSongLoadedEvent(const juce::String& songId)
{
    // Create and publish song loaded event
    auto event = std::make_shared<SongLoadedEvent>(songId);
    EventBus::getInstance().publishOnMainThread(event);
    
    DBG("GuitarPracticeController: Published SongLoadedEvent for song: " + songId);
}

void GuitarPracticeController::publishSongLoadFailedEvent(const juce::String& songId, const juce::String& errorMessage)
{
    // Create and publish song load failed event
    auto event = std::make_shared<SongLoadFailedEvent>(songId, errorMessage);
    EventBus::getInstance().publishOnMainThread(event);
    
    DBG("GuitarPracticeController: Published SongLoadFailedEvent for song: " + songId + ", error: " + errorMessage);
}