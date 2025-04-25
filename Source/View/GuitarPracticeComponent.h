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

// 녹음 기능을 위한 전방 선언
class AudioRecorder;
class RecordingThumbnail;

class MainComponent;  // 전방 선언

// GuitarPracticeComponent - 기타 연습 기능의 View 역할
// MVC 패턴 중 View 역할만 담당하며, 비즈니스 로직은 Controller에 위임
class GuitarPracticeComponent : public juce::Component,
                               public IAudioModelListener,  // 오디오 모델 리스너
                               public IEventListener       // 이벤트 시스템 리스너
{
public:
    GuitarPracticeComponent(MainComponent& mainComponent);
    ~GuitarPracticeComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // IAudioModelListener 인터페이스 구현
    void onPlayStateChanged(bool isPlaying) override;
    void onVolumeChanged(float newVolume) override;
    void onPositionChanged(double positionInSeconds) override;
    void onInputLevelChanged(float newLevel) override;
    
    // IEventListener 인터페이스 구현
    bool onEvent(const Event& event) override;
    
    // 곡 선택 관련 메서드 (UI에서 호출됨)
    void loadSong(const juce::String& songId);
    
    // 녹음 관련 메서드 (UI에서 호출됨)
    void startRecording();
    void stopRecording();
    bool isRecording() const;
    
    // 오디오 분석 메서드 (UI에서 호출됨)
    void analyzeRecording();
    
    // UI 업데이트 메서드
    void updateUI();
    
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

    // 녹음 관련 컴포넌트
    std::unique_ptr<RecordingThumbnail> recordingThumbnail;
    std::unique_ptr<AudioRecorder> audioRecorder;
    juce::AudioFormatManager formatManager;
    juce::TextButton recordButton;
    juce::TextButton analyzeButton;
    juce::File lastRecording;
    
    // UI 컨트롤
    juce::TextButton playButton;
    juce::Label positionLabel;
};