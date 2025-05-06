#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"
#include "Model/IAudioModelListener.h"
#include "Controller/GuitarPracticeController.h"
#include "Controller/AudioController.h"
#include "Event/IEventListener.h"
#include "TopBar.h"
#include "CenterPanel.h"
#include "LeftPanel.h"
#include "RightPanel.h"
#include "ScoreComponent.h"
#include "PracticeSettingsComponent.h"
#include "LookAndFeel/MapleTheme.h"

// 녹음 기능을 위한 전방 선언
class AudioRecorder;
class RecordingThumbnail;

// LookAndFeel 클래스들 전방 선언
class MapleRecordButton;
class MapleAnalyzeButton; 
class MaplePlayButton; // Play 버튼용 LookAndFeel 클래스 선언 추가

// 새로운 컴포넌트 추가
class PerformanceAnalysisComponent;
class FingeringGuideComponent;
class PracticeProgressComponent;

class MainComponent;  // 전방 선언

// GuitarPracticeComponent - 기타 연습 기능의 View 역할
// MVC 패턴 중 View 역할만 담당하며, 비즈니스 로직은 Controller에 위임
class GuitarPracticeComponent : public juce::Component,
                               public IAudioModelListener,  // 오디오 모델 리스너
                               public IEventListener,      // 이벤트 시스템 리스너
                               public juce::Timer,         // 애니메이션을 위한 타이머 추가
                               public PlaybackStateChangeListener // ScoreComponent의 재생 상태 변경 리스너
{
public:
    // 표시할 악보 아래 컴포넌트 유형
    enum class BottomViewType {
        PerformanceAnalysis,
        FingeringGuide,
        PracticeProgress,
        None
    };
    
    GuitarPracticeComponent(MainComponent& mainComponent);
    ~GuitarPracticeComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress& key) override; // 키보드 입력 처리
    
    // IAudioModelListener 인터페이스 구현
    void onPlayStateChanged(bool isPlaying) override;
    void onVolumeChanged(float newVolume) override;
    void onPositionChanged(double positionInSeconds) override;
    void onInputLevelChanged(float newLevel) override;
    
    // IEventListener 인터페이스 구현
    bool onEvent(const Event& event) override;
    
    // PlaybackStateChangeListener 인터페이스 구현
    void onPlaybackStateChanged(bool isPlaying) override;
    
    // Timer 인터페이스 구현
    void timerCallback() override;
    
    // 곡 선택 관련 메서드 (UI에서 호출됨)
    void loadSong(const juce::String& songId);
    
    // 녹음 관련 메서드 (UI에서 호출됨)
    void startRecording();
    void stopRecording();
    bool isRecording() const;
    
    // 오디오 분석 메서드 (UI에서 호출됨)
    void analyzeRecording();
    
    // 마이크 모니터링 관련 메서드
    void enableMicrophoneMonitoring(bool shouldEnable);
    bool isMicrophoneMonitoringEnabled() const { return microphoneMonitoringEnabled; }
    void toggleMicrophoneMonitoring();
    
    // UI 업데이트 메서드
    void updateUI();
    
    // 하단 뷰 전환 메서드
    void switchBottomView(BottomViewType viewType);
    
    // 현재 하단 뷰 가져오기
    BottomViewType getCurrentBottomView() const { return currentBottomView; }
    
    // 오디오 디바이스 매니저 접근자
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }
    
    // 컨트롤러 접근자
    GuitarPracticeController* getController() const { return controller.get(); }
    
private:
    // 이벤트 핸들러 메서드
    void handleAnalysisCompleteEvent(const AnalysisCompleteEvent& event);
    void handleAnalysisFailedEvent(const AnalysisFailedEvent& event);
    void handleSongLoadedEvent(const SongLoadedEvent& event);
    void handleSongLoadFailedEvent(const SongLoadFailedEvent& event);
    
    // View에서는 UI 관련 작업만 처리
    void updatePlaybackState(bool isNowPlaying);
    void updateVolumeDisplay(float volume);
    void updatePositionDisplay(double positionInSeconds);
    void togglePlayback();  // UI 이벤트 핸들러
    
    // 하단 뷰 가시성 업데이트
    void updateBottomViewVisibility();
    
    // 뷰 전환 버튼 상태 업데이트
    void updateViewButtonStates();

    MainComponent& mainComponent;
    
    // 오디오 디바이스 관리자 참조
    juce::AudioDeviceManager& deviceManager;
    
    // MVC 컴포넌트들
    AudioModel audioModel;
    std::unique_ptr<GuitarPracticeController> controller;
    std::unique_ptr<AudioController> audioController;
    
    // View 컴포넌트들 (직접 참조)
    std::unique_ptr<TopBar> topBar;
    std::unique_ptr<CenterPanel> centerPanel;
    std::unique_ptr<LeftPanel> leftPanel;
    std::unique_ptr<RightPanel> rightPanel;
    std::unique_ptr<ScoreComponent> scoreComponent;
    std::unique_ptr<PracticeSettingsComponent> practiceSettingsComponent;
    
    // 새로운 컴포넌트들
    std::unique_ptr<PerformanceAnalysisComponent> performanceAnalysisComponent;
    std::unique_ptr<FingeringGuideComponent> fingeringGuideComponent;
    std::unique_ptr<PracticeProgressComponent> practiceProgressComponent;

    // 녹음 관련 컴포넌트
    std::unique_ptr<RecordingThumbnail> recordingThumbnail;
    std::unique_ptr<AudioRecorder> audioRecorder;
    juce::AudioFormatManager formatManager;
    juce::TextButton recordButton;
    juce::TextButton analyzeButton;
    juce::File lastRecording;
    
    // 커스텀 LookAndFeel 인스턴스
    std::unique_ptr<MapleRecordButton> recordButtonLookAndFeel;
    std::unique_ptr<MapleAnalyzeButton> analyzeButtonLookAndFeel;
    std::unique_ptr<MaplePlayButton> playButtonLookAndFeel; // Play 버튼용 LookAndFeel 추가
    
    // UI 컨트롤
    juce::TextButton playButton; // 세련된 디자인이 적용될 예정
    juce::Label positionLabel;
    
    // 뷰 전환 버튼
    juce::TextButton analysisViewButton;
    juce::TextButton fingeringViewButton;
    juce::TextButton progressViewButton;
    
    // 마이크 모니터링 관련 컨트롤
    juce::ToggleButton micMonitorButton;
    bool microphoneMonitoringEnabled = false;
    
    // 현재 표시 중인 하단 뷰
    BottomViewType currentBottomView = BottomViewType::PerformanceAnalysis;
};