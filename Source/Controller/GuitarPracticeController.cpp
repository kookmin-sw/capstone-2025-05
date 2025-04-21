#include "GuitarPracticeController.h"
#include "../GuitarPracticeComponent.h"
#include "../EnvLoader.h"

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
    audioModel.setPlaying(true);
    player.startPlaying();
}

void GuitarPracticeController::stopPlayback()
{
    audioModel.setPlaying(false);
    player.stopPlaying();
}

void GuitarPracticeController::togglePlayback()
{
    if (!audioModel.isPlaying())
    {
        startPlayback();
    }
    else
    {
        stopPlayback();
    }
}

bool GuitarPracticeController::loadSong(const juce::String& songId)
{
    // 서버가 준비되지 않았으므로 로컬 파일 경로를 사용하여 테스트
    juce::String filePath;
    
    // 현재 디버깅 중인 위치와 현재 경로 로깅
    DBG("Current working directory: " + juce::File::getCurrentWorkingDirectory().getFullPathName());
    
    // 테스트용 곡 ID와 경로 매핑
    if (songId == "song1")
        filePath = "D:/audio_dataset/recording/homecoming/homecoming.gp5";
    else if (songId == "song2")
        filePath = "D:/audio_dataset/recording/song2/song2.gp5";
    else if (songId == "song3")
        filePath = "D:/audio_dataset/recording/song3/song3.gp5";
    else if (songId == "song4")
        filePath = "D:/audio_dataset/recording/song4/song4.gp5";
    else if (songId == "song5")
        filePath = "D:/audio_dataset/recording/song5/song5.gp5";
    else
        return false; // 지원하지 않는 곡 ID
    
    // 파일 존재 여부 확인
    juce::File gpFile(filePath);
    if (!gpFile.existsAsFile()) {
        DBG("GP file does not exist: " + filePath);
        
        // 빈 TabFile 설정
        player.setTabFile(gp_parser::TabFile(major, minor, title, subtitle, artist, album,
                                           lyricsAuthor, musicAuthor, copyright, tab, instructions,
                                           comments, lyric, tempoValue, globalKeySignature, channels,
                                           measures, trackCount, measureHeaders, tracks));
        
        return true;
    }
    
    resetParser();
    
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
        }
        catch (const std::exception& e) {
            DBG("Error in parser.getTabFile() or player.setTabFile(): " + juce::String(e.what()));
            return false;
        }
        
        DBG("Song loaded successfully: " + songId);
        return true;
    }
    catch (const std::exception& e) {
        DBG("Error loading song: " + juce::String(e.what()));
        return false;
    }
    catch (...) {
        DBG("Unknown error loading song");
        return false;
    }
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
    
    // 기존 스레드 정리
    currentAnalysisThread = nullptr;
    
    // 새 분석 스레드 생성
    currentAnalysisThread = std::make_unique<AnalysisThread>(serverUrl, recordingFile);
    
    // 프로그레스 창 메시지 설정
    currentAnalysisThread->setStatusMessage("Preparing analysis...");
    
    // 스레드 시작 (비동기식)
    currentAnalysisThread->launchThread();
    
    // 디버깅 정보
    DBG("[ANALYSIS] Thread launched");
    
    // 타이머 시작
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
        
        if (view)
        {
            juce::MessageManager::callAsync([this]() {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Analysis Cancelled",
                    "The analysis was cancelled.",
                    "OK"
                );
            });
        }
    }
    else if (currentAnalysisThread->analysisCompleted)
    {
        DBG("[ANALYSIS] Thread completed successfully");
        
        if (currentAnalysisThread->statusJson.isVoid())
        {
            DBG("[ANALYSIS] ERROR: statusJson is void!");
            
            if (view)
            {
                juce::MessageManager::callAsync([this]() {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Analysis Failed",
                        "The analysis completed but no data was returned.",
                        "OK"
                    );
                });
            }
        }
        else
        {
            DBG("[ANALYSIS] Successful analysis detected");
            
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
    
    // 스레드 객체 정리
    currentAnalysisThread = nullptr;
}

// AnalysisTimer 타이머 콜백 구현
void GuitarPracticeController::AnalysisTimer::timerCallback()
{
    // 분석 스레드가 없으면 타이머 중지
    if (!controller.currentAnalysisThread)
    {
        stopTimer();
        return;
    }
    
    // 스레드가 실행 중이 아니면 (완료되었거나 취소됨)
    if (!controller.currentAnalysisThread->isThreadRunning())
    {
        DBG("[ANALYSIS] Thread is no longer running - checking results");
        controller.handleAnalysisThreadComplete();
        stopTimer();
    }
}