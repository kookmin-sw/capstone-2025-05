#include "GuitarPracticeController.h"
#include "View/GuitarPracticeComponent.h"
#include "Util/EnvLoader.h"
#include "API/SongsAPIService.h"
#include "Event/Event.h"

GuitarPracticeController::GuitarPracticeController(AudioModel& audioModel, juce::AudioDeviceManager& deviceManager)
    : audioModel(audioModel), deviceManager(deviceManager), contentController(nullptr), player()
{
    DBG("GuitarPracticeController - constructor start");
    
    // 오디오 포맷 매니저 초기화
    formatManager.registerBasicFormats();
    
    // MP3 포맷이 등록되었는지 확인
    bool mp3Supported = false;
    for (int i = 0; i < formatManager.getNumKnownFormats(); ++i)
    {
        auto* format = formatManager.getKnownFormat(i);
        if (format->getFormatName().containsIgnoreCase("mp3"))
        {
            mp3Supported = true;
            break;
        }
    }
    
    // 지원되는 모든 포맷 로깅
    juce::StringArray supportedFormats;
    for (int i = 0; i < formatManager.getNumKnownFormats(); ++i)
    {
        auto* format = formatManager.getKnownFormat(i);
        supportedFormats.add(format->getFormatName());
    }
    DBG("GuitarPracticeController - Supported audio formats: " + supportedFormats.joinIntoString(", "));
    
    if (!mp3Supported)
    {
        DBG("GuitarPracticeController - WARNING: MP3 format is not supported!");
    }
    
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
    juce::String apiUrl = EnvLoader::get("MAPLE_MEDIA_API_URL");
    if (apiUrl.isNotEmpty())
    {
        apiService->setApiBaseUrl(apiUrl);
        DBG("GuitarPracticeController: API URL from environment: " + apiUrl);
    }
    else
    {
        DBG("GuitarPracticeController: Using default API URL from SongsAPIService");
    }
    
    #ifdef JUCE_DEBUG
    // 디버그 모드에서는 localhost 사용
    // apiService->setApiBaseUrl("http://localhost:8000");
    #endif
    
    DBG("GuitarPracticeController: API URL set to: " + apiService->getApiBaseUrl());
    
    DBG("GuitarPracticeController - constructor complete");
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
    DBG("GuitarPracticeController::startPlayback - starting playback");
    
    // 1. 먼저 상태를 업데이트하여 UI가 빠르게 반응하도록 함
    // 메인 스레드에서 실행
    audioModel.setPlaying(true);
    
    // 1.5 TabPlayer 즉시 시작 (UI 응답성을 위해 메인 스레드에서 실행)
    player.startPlaying();
    
    // 2. 무거운 오디오 처리는 백그라운드에서 처리
    juce::Thread::launch([this]() {
        try {
            // 오디오 파일 재생 (reader가 있는 경우에만)
            if (readerSource != nullptr)
            {
                DBG("GuitarPracticeController::startPlayback - Background thread: Starting transportSource");
                
                // 재생 위치를 처음으로 설정
                transportSource.setPosition(0.0);
                
                // 명시적으로 start() 호출
                transportSource.start();
                
                // 실제로 재생되었는지 확인
                if (transportSource.isPlaying())
                    DBG("GuitarPracticeController::startPlayback - Background thread: transportSource started");
                else
                    DBG("GuitarPracticeController::startPlayback - Background thread: transportSource start failed");
            }
            else
            {
                DBG("GuitarPracticeController::startPlayback - Background thread: No audio file to play");
            }
            
            // 디버깅을 위한 상태 로깅
            DBG("GuitarPracticeController::startPlayback - Background thread: Current playback state:");
            DBG("  player.isPlaying() = " + juce::String(player.isPlaying() ? "true" : "false"));
            DBG("  transportSource.isPlaying() = " + juce::String(transportSource.isPlaying() ? "true" : "false"));
            DBG("  audioModel.isPlaying() = " + juce::String(audioModel.isPlaying() ? "true" : "false"));
        }
        catch (const std::exception& e) {
            DBG("GuitarPracticeController::startPlayback - Background thread exception: " + juce::String(e.what()));
            
            // 실패 시 롤백 작업이 필요하면 메인 스레드에서 해야 함
            juce::MessageManager::callAsync([this]() {
                if (player.isPlaying()) {
                    player.stopPlaying();
                }
                audioModel.setPlaying(false);
                DBG("GuitarPracticeController::startPlayback - Rolled back playback state due to error");
            });
        }
    });
}

void GuitarPracticeController::stopPlayback()
{
    DBG("GuitarPracticeController::stopPlayback - stopping playback");
    
    // 디버깅을 위한 현재 상태 기록
    bool wasPlaying = player.isPlaying() || transportSource.isPlaying();
    
    // 1. 먼저 상태를 비동기적으로 업데이트하여 UI가 빠르게 반응하도록 함
    // 이것은 UI에서 즉시 반영되어야 하므로 메인 스레드에서 실행
    audioModel.setPlaying(false);
    
    // 1.5 TabPlayer를 즉시 중지하여 소리가 즉시 중지되게 함
    player.stopPlaying();
    
    // 2. 리소스 해제 작업은 백그라운드 스레드에서 수행 (무거운 작업)
    // 이 작업이 메인 스레드를 차단하는 것을 방지
    juce::Thread::launch([this, wasPlaying]() {
        try {
            // 오디오 파일 재생 정지 (TransportSource)
            // 해제 시간이 오래 걸릴 수 있는 부분 - 백그라운드에서 처리
            transportSource.stop();
            
            DBG("GuitarPracticeController::stopPlayback - Background thread: Transport stopped");
        }
        catch (const std::exception& e) {
            DBG("GuitarPracticeController::stopPlayback - Exception: " + juce::String(e.what()));
        }
        
        // 디버깅을 위한 상태 로깅
        DBG("GuitarPracticeController::stopPlayback - Background thread: Playback stopped");
        DBG("  Was playing before: " + juce::String(wasPlaying ? "true" : "false"));
        DBG("  player.isPlaying() = " + juce::String(player.isPlaying() ? "true" : "false"));
        DBG("  transportSource.isPlaying() = " + juce::String(transportSource.isPlaying() ? "true" : "false"));
        DBG("  audioModel.isPlaying() = " + juce::String(audioModel.isPlaying() ? "true" : "false"));
    });
}

void GuitarPracticeController::togglePlayback()
{
    // 현재 재생 상태 확인 (한 번만 확인)
    bool isCurrentlyPlaying = audioModel.isPlaying();
    
    DBG("GuitarPracticeController::togglePlayback - Current state: " + 
        juce::String(isCurrentlyPlaying ? "playing -> stopping" : "stopped -> starting"));
    
    // 적절한 메서드 직접 호출 (불필요한 상태 확인 제거)
    if (isCurrentlyPlaying)
        stopPlayback();
    else
        startPlayback();
    
    // 토글 후 최종 상태 확인 (간소화된 로깅)
    DBG("GuitarPracticeController::togglePlayback - New state: " + 
        juce::String(audioModel.isPlaying() ? "playing" : "stopped"));
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
        
        // 오디오 URL 추출
        juce::String audioUrl = response.data.getProperty("audio", "").toString();
        
        // 오디오 URL이 비어있고, 곡 ID가 있는 경우 API 엔드포인트 직접 구성
        if (audioUrl.isEmpty() && !songId.isEmpty())
        {
            // ID 기반 오디오 경로 설정
            audioUrl = "/songs/" + songId + "/audio";
            DBG("Generated audio URL from ID: " + audioUrl);
        }
        
        // 악보 데이터 URL 엔드포인트 직접 구성 (선택 사항)
        juce::String scoreEndpoint = "/songs/" + songId + "/sheet";
        
        // 오디오 서비스의 downloadAudioFile 메서드 사용 - 악보 파일도 바이너리 파일이므로 이 메서드 활용
        apiService->downloadAudioFile(sheetMusicUrl, cacheFile, [this, songId, audioUrl](bool success, juce::String filePath) {
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
                    // 파서에서 TabFile 가져오기
                    const gp_parser::TabFile& tabFile = parserPtr->getTabFile();
                    
                    // TabPlayer에 TabFile 설정
                    player.setTabFile(tabFile);
                    
                    // 오디오 파일 URL이 있는 경우 다운로드 시작
                    if (audioUrl.isNotEmpty())
                    {
                        DBG("Found audio URL: " + audioUrl);
                        
                        // 오디오 캐시 디렉토리 생성
                        juce::File audioCacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                                   .getChildFile("MapleClientDesktop/cache/audio");
                        if (!audioCacheDir.exists())
                            audioCacheDir.createDirectory();
                        
                        // 오디오 파일명 생성 - WAV 확장자 사용 (서버에서 WAV 파일을 제공)
                        juce::String audioFileName = songId + "_audio.wav";
                        juce::File audioFile = audioCacheDir.getChildFile(audioFileName);
                        
                        DBG("Downloading audio from URL: " + audioUrl + " to: " + audioFile.getFullPathName());
                        
                        // 새로운 downloadSongAudio 메소드를 사용하여 곡 ID로 직접 오디오 다운로드
                        apiService->downloadSongAudio(songId, audioFile, [this, songId](bool audioSuccess, juce::String audioFilePath) {
                            if (audioSuccess)
                            {
                                DBG("Audio file downloaded to: " + audioFilePath);
                                
                                // 음원 파일 로드
                                juce::File downloadedAudioFile(audioFilePath);
                                if (loadAudioFile(downloadedAudioFile))
                                {
                                    DBG("Audio file loaded successfully");
                                }
                                else
                                {
                                    DBG("Failed to load audio file: " + audioFilePath);
                                }
                                
                                // 악보 로드 성공 이벤트 발생
                                publishSongLoadedEvent(songId);
                            }
                            else
                            {
                                DBG("Failed to download audio for song ID: " + songId);
                                
                                // 오디오 다운로드 실패해도 악보만으로도 계속 진행
                                publishSongLoadedEvent(songId);
                            }
                        });
                    }
                    else
                    {
                        DBG("No audio URL found, proceeding with score only");
                        publishSongLoadedEvent(songId);
                    }
                    
                    DBG("Song loaded successfully: " + songId);
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
    // View에서 AudioRecorder 객체를 관리하므로 실제 View에 상태를 문의
    if (view != nullptr) {
        return view->isRecording();
    }
    return false;
}

void GuitarPracticeController::startRecording()
{
    // 재생 중이면 중지
    if (audioModel.isPlaying())
        togglePlayback();
    
    // 실제 녹음 시작은 View에서 처리
    if (view != nullptr) {
        view->startRecording();
        DBG("GuitarPracticeController::startRecording - delegated to View");
    } else {
        DBG("GuitarPracticeController::startRecording - view is nullptr!");
    }
}

void GuitarPracticeController::stopRecording()
{
    // 실제 녹음 중지는 View에서 처리
    if (view != nullptr) {
        view->stopRecording();
        DBG("GuitarPracticeController::stopRecording - delegated to View");
    } else {
        DBG("GuitarPracticeController::stopRecording - view is nullptr!");
    }
}

// 분석 스레드 클래스 선언
class GuitarPracticeController::AnalysisThread : public juce::ThreadWithProgressWindow
{
public:
    AnalysisThread(const juce::String& url, const juce::File& recording, const juce::String& songId)
        : juce::ThreadWithProgressWindow("Analyzing Recording...", true, true),
          serverUrl(url),
          audioFile(recording),
          currentSongId(songId)
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
            url = url.withParameter("song_id", currentSongId);
            url = url.withParameter("generate_feedback", "true");
            
            DBG("Full URL with parameters: " + url.toString(true));
            
            // 멀티파트 폼 데이터 구성을 간소화
            juce::StringPairArray responseHeaders;
            int statusCode = 0;
            
            // POST 요청 생성 (파일 첨부)
            url = url.withFileToUpload("user_file", audioFile, "audio/wav");
            
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
                juce::URL statusUrl(serverUrl.upToLastOccurrenceOf("/compare-with-reference", false, false) + 
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
    juce::String currentSongId;
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
    
    juce::String serverUrl = EnvLoader::get("MAPLE_ANALYSIS_API_URL") + "compare-with-reference";
    
    #ifdef JUCE_DEBUG
    serverUrl = "http://localhost:8000/api/v1/compare-with-reference";
    #endif
    
    // Clean up existing thread
    currentAnalysisThread = nullptr;
    
    // Create new analysis thread with current song ID
    currentAnalysisThread = std::make_unique<AnalysisThread>(serverUrl, recordingFile, currentSongId);
    
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

// 새로 추가된 오디오 파일 관련 메서드
bool GuitarPracticeController::loadAudioFile(const juce::File& audioFile)
{
    DBG("GuitarPracticeController::loadAudioFile - loading file: " + audioFile.getFullPathName());
    
    // 파일이 존재하는지 확인
    if (!audioFile.existsAsFile())
    {
        DBG("GuitarPracticeController::loadAudioFile - file does not exist: " + audioFile.getFullPathName());
        return false;
    }
    
    // 파일 크기 확인
    auto fileSize = audioFile.getSize();
    if (fileSize <= 0)
    {
        DBG("GuitarPracticeController::loadAudioFile - file exists but has zero size: " + audioFile.getFullPathName());
        return false;
    }
    
    DBG("GuitarPracticeController::loadAudioFile - file size: " + juce::String(fileSize) + " bytes");
    
    // 지원되는 포맷 확인 (미리 로깅)
    juce::StringArray formats;
    for (int i = 0; i < formatManager.getNumKnownFormats(); ++i)
    {
        auto* format = formatManager.getKnownFormat(i);
        formats.add(format->getFormatName());
    }
    DBG("GuitarPracticeController::loadAudioFile - Supported formats: " + formats.joinIntoString(", "));
    
    // 파일 포맷이 WAV인지 확인
    juce::String extension = audioFile.getFileExtension().toLowerCase();
    DBG("GuitarPracticeController::loadAudioFile - File extension: " + extension);
    
    // WAV 파일이 아니라면 경고 표시 (항상 WAV 파일이어야 함)
    if (extension != ".wav")
    {
        DBG("GuitarPracticeController::loadAudioFile - Warning: File is not a WAV file. Only WAV files are fully supported.");
    }
    
    // 기존 소스 제거
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
    
    // AudioFormatManager가 제대로 초기화되었는지 확인
    if (formatManager.getNumKnownFormats() == 0)
    {
        DBG("GuitarPracticeController::loadAudioFile - Warning: No registered formats! Registering now...");
        formatManager.registerBasicFormats();
        
        // WAV 포맷이 등록되었는지 다시 확인
        bool wavFormatFound = false;
        for (int i = 0; i < formatManager.getNumKnownFormats(); ++i)
        {
            auto* format = formatManager.getKnownFormat(i);
            if (format->getFormatName().containsIgnoreCase("wav") || 
                format->getFormatName().containsIgnoreCase("wave"))
            {
                wavFormatFound = true;
                break;
            }
        }
        
        if (!wavFormatFound)
        {
            DBG("GuitarPracticeController::loadAudioFile - ERROR: WAV format not registered!");
        }
        
        // 다시 지원되는 포맷 로깅
        juce::StringArray updatedFormats;
        for (int i = 0; i < formatManager.getNumKnownFormats(); ++i)
        {
            auto* format = formatManager.getKnownFormat(i);
            updatedFormats.add(format->getFormatName());
        }
        DBG("GuitarPracticeController::loadAudioFile - Updated supported formats: " + updatedFormats.joinIntoString(", "));
    }
    
    // 오디오 파일 열어보기 시도
    juce::File originalFile = audioFile;
    DBG("GuitarPracticeController::loadAudioFile - Attempting to read: " + originalFile.getFullPathName());
    
    // 파일 내용 확인을 위한 기본 체크
    {
        juce::FileInputStream fileStream(originalFile);
        if (fileStream.openedOk())
        {
            // WAV 헤더 체크 (간단한 검증)
            char header[16] = {0};
            fileStream.read(header, 16);
            
            // RIFF/WAVE 헤더 검사
            bool hasValidHeader = std::memcmp(header, "RIFF", 4) == 0 && 
                                std::memcmp(header + 8, "WAVE", 4) == 0;
            
            DBG("GuitarPracticeController::loadAudioFile - WAV header check: " + juce::String(hasValidHeader ? "Valid" : "Invalid"));
            
            // 첫 16바이트 출력
            juce::String headerBytes;
            for (int i = 0; i < 16; i++) {
                char c = header[i];
                if (c >= 32 && c <= 126) // 출력 가능한 ASCII 문자
                    headerBytes += c;
                else
                    headerBytes += juce::String::formatted("[%02X]", (unsigned char)c);
            }
            DBG("GuitarPracticeController::loadAudioFile - File header: " + headerBytes);
        }
        else
        {
            DBG("GuitarPracticeController::loadAudioFile - Could not open file for header check!");
        }
    }
    
    // 오디오 파일 읽기
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(originalFile));
    
    if (reader != nullptr)
    {
        // 샘플 레이트 및 채널 정보를 미리 저장 (reader release 전에)
        const double readerSampleRate = reader->sampleRate;
        const int numChannels = reader->numChannels;
        const juce::int64 lengthInSamples = reader->lengthInSamples;
        const int bitsPerSample = reader->bitsPerSample;
        
        DBG("GuitarPracticeController::loadAudioFile - reader created successfully");
        DBG("  Sample rate: " + juce::String(readerSampleRate));
        DBG("  Num channels: " + juce::String(numChannels));
        DBG("  Length in samples: " + juce::String(lengthInSamples));
        DBG("  Total length in seconds: " + juce::String(lengthInSamples / readerSampleRate));
        DBG("  Bit depth: " + juce::String(bitsPerSample));
        
        // reader에서 AudioFormatReaderSource 생성
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
        
        // transportSource에 설정 (저장해둔 샘플 레이트 사용)
        transportSource.setSource(readerSource.get(), 0, nullptr, readerSampleRate);
        
        // 현재 오디오 디바이스의 샘플 레이트와 버퍼 크기 가져오기
        auto currentDevice = deviceManager.getCurrentAudioDevice();
        if (currentDevice != nullptr)
        {
            double sampleRate = currentDevice->getCurrentSampleRate();
            int samplesPerBlock = currentDevice->getCurrentBufferSizeSamples();
            
            // prepareToPlay 호출하여 초기화
            transportSource.prepareToPlay(samplesPerBlock, sampleRate);
            
            DBG(juce::String("GuitarPracticeController::loadAudioFile - transportSource prepared with ") +
                "sampleRate = " + juce::String(sampleRate) + ", samplesPerBlock = " + juce::String(samplesPerBlock));
        }
        else
        {
            DBG("GuitarPracticeController::loadAudioFile - WARNING: No current audio device!");
            
            // 기본값으로 초기화
            transportSource.prepareToPlay(512, 44100.0);
            DBG("GuitarPracticeController::loadAudioFile - Using default values: sampleRate = 44100, samplesPerBlock = 512");
        }
        
        // 현재 파일 저장
        currentAudioFile = originalFile;
        
        // 정상 로드 확인
        if (readerSource != nullptr)
        {
            DBG("GuitarPracticeController::loadAudioFile - readerSource is valid");
            warnedAboutNoSource = false; // 새 소스가 로드되면 경고 초기화
        }
        
        DBG("GuitarPracticeController::loadAudioFile - file loaded successfully");
        return true;
    }
    else
    {
        DBG("GuitarPracticeController::loadAudioFile - FAILED to create reader for: " + originalFile.getFullPathName());
        
        // 파일 타입 확인
        DBG("  File type: " + extension);
        
        // 파일 형식에 대한 문제 진단
        if (extension == ".wav")
        {
            DBG("  This is a WAV file but still failed to load. Possible reasons:");
            DBG("  - Corrupted WAV file");
            DBG("  - Unsupported bit depth or encoding");
            DBG("  - Unsupported channel configuration");
            
            // 추가 디버깅을 위해 파일의 첫 부분 체크
            juce::FileInputStream fileStream(originalFile);
            if (fileStream.openedOk())
            {
                // WAV 헤더 체크 (간단한 검증)
                char header[12];
                fileStream.read(header, 12);
                
                // RIFF/WAVE 헤더 검사
                bool hasValidHeader = std::memcmp(header, "RIFF", 4) == 0 && 
                                     std::memcmp(header + 8, "WAVE", 4) == 0;
                
                DBG("  WAV header valid: " + juce::String(hasValidHeader ? "Yes" : "No"));
            }
        }
        else
        {
            DBG("  This is not a WAV file. The app is configured to use WAV files only.");
        }
        
        return false;
    }
}

void GuitarPracticeController::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void GuitarPracticeController::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // 로그 출력 빈도 조정 (100 = 약 1초)
    static int callbackCounter = 0;
    bool shouldLog = (++callbackCounter >= 100);
    if (shouldLog)
        callbackCounter = 0;
    
    if (readerSource != nullptr)
    {
        // TransportSource가 재생 중인지 확인
        bool isPlaying = transportSource.isPlaying();
        
        if (shouldLog)
            DBG("GuitarPracticeController::getNextAudioBlock - transportSource.isPlaying() = " + 
                juce::String(isPlaying ? "true" : "false"));
        
        // 재생 중인 경우에만 오디오 블록 가져오기
        if (isPlaying)
        {
            // 오디오 데이터 출력
            transportSource.getNextAudioBlock(bufferToFill);
            
            // 버퍼에 실제 오디오 데이터가 있는지 확인
            if (shouldLog)
            {
                bool hasAudioData = false;
                float maxLevel = 0.0f;
                
                for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch)
                {
                    const float* channelData = bufferToFill.buffer->getReadPointer(ch, bufferToFill.startSample);
                    
                    for (int i = 0; i < bufferToFill.numSamples; ++i)
                    {
                        maxLevel = juce::jmax(maxLevel, std::abs(channelData[i]));
                        if (std::abs(channelData[i]) > 0.0001f)
                            hasAudioData = true;
                    }
                }
                
                DBG("GuitarPracticeController::getNextAudioBlock - has audio data = " + 
                    juce::String(hasAudioData ? "true" : "false") +
                    ", max level = " + juce::String(maxLevel));
            }
        }
        else
        {
            // 재생 중이 아니면 무음 출력
            bufferToFill.clearActiveBufferRegion();
            
            if (shouldLog)
                DBG("GuitarPracticeController::getNextAudioBlock - transportSource not playing, clearing buffer");
        }
    }
    else
    {
        // 소스가 없으면 무음 출력
        bufferToFill.clearActiveBufferRegion();
        
        // 소스가 없다는 로그는 처음 한 번만 출력
        if (!warnedAboutNoSource)
        {
            warnedAboutNoSource = true;
            DBG("GuitarPracticeController::getNextAudioBlock - No valid audio source");
        }
        else if (shouldLog)
        {
            DBG("GuitarPracticeController::getNextAudioBlock - Still no valid audio source");
        }
    }
}

void GuitarPracticeController::releaseResources()
{
    transportSource.releaseResources();
}