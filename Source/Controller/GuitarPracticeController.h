#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"
#include "Model/TabPlayer.h"
#include "Model/gp_parser.h"
#include "Event/EventBus.h"

// 전방 선언
class GuitarPracticeComponent;

/**
 * GuitarPracticeController - 기타 연습 기능의 Controller 역할
 * 
 * View(GuitarPracticeComponent)에서 Controller 로직을 분리하여 
 * Model과 View 사이의 중개자 역할을 담당합니다.
 */
class GuitarPracticeController {
public:
    // 분석 스레드 클래스에 대한 전방 선언
    class AnalysisThread;
    
    GuitarPracticeController(AudioModel& model, juce::AudioDeviceManager& deviceManager);
    ~GuitarPracticeController();
    
    // 곡 선택 관련 메서드
    bool loadSong(const juce::String& songId);
    void resetParser();
    
    // 재생 제어 메서드
    void startPlayback();
    void stopPlayback();
    void togglePlayback();
    
    // 녹음 관련 메서드
    void startRecording();
    void stopRecording();
    juce::File getLastRecordingFile() const { return lastRecordingFile; }
    bool isRecording() const;
    
    // 오디오 분석 메서드
    void analyzeRecording(const juce::File& recordingFile);
    
    // View 참조 설정 (순환 참조 방지를 위해 약한 참조 사용)
    void setView(GuitarPracticeComponent* view) { this->view = view; }
    
    // TabPlayer 접근자
    TabPlayer& getTabPlayer() { return player; }
    
private:
    // 분석 스레드 결과 처리 메서드
    void handleAnalysisThreadComplete();
    
    // 이벤트 발행 헬퍼 메서드
    void publishAnalysisCompleteEvent(const juce::var& result);
    void publishAnalysisFailedEvent(const juce::String& errorMessage);
    void publishSongLoadedEvent(const juce::String& songId);
    void publishSongLoadFailedEvent(const juce::String& songId, const juce::String& errorMessage);
    
    // 모델 및 뷰 참조
    AudioModel& audioModel;
    juce::AudioDeviceManager& deviceManager;
    GuitarPracticeComponent* view = nullptr; // 약한 참조
    
    // Guitar Pro 파서 및 플레이어
    std::unique_ptr<gp_parser::Parser> parserPtr;
    TabPlayer player;
    
    // 녹음 관련 상태
    juce::File lastRecordingFile;
    
    // 분석 스레드
    std::unique_ptr<AnalysisThread> currentAnalysisThread;
    
    // TabFile 변수들 (loadSong에서 사용)
    std::int32_t major = 0;
    std::int32_t minor = 0;
    std::string title = "";
    std::string subtitle = "";
    std::string artist = "";
    std::string album = "";
    std::string lyricsAuthor = "";
    std::string musicAuthor = "";
    std::string copyright = "";
    std::string tab = "";
    std::string instructions = "";
    std::vector<std::string> comments;
    gp_parser::Lyric lyric = {0, ""};
    std::int32_t tempoValue = 120;
    std::int8_t globalKeySignature = 0;
    std::vector<gp_parser::Channel> channels;
    std::int32_t measures = 0;
    std::int32_t trackCount = 0;
    std::vector<gp_parser::MeasureHeader> measureHeaders;
    std::vector<gp_parser::Track> tracks;
    
    // 타이머 콜백을 위한 JUCE 타이머
    class AnalysisTimer : public juce::Timer {
    public:
        AnalysisTimer(GuitarPracticeController& controller) : controller(controller) {}
        void timerCallback() override;
    private:
        GuitarPracticeController& controller;
    };
    
    std::unique_ptr<AnalysisTimer> analysisTimer;
};