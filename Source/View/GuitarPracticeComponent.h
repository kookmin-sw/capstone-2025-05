#pragma once
#include <JuceHeader.h>
#include "../Model/AudioModel.h"
#include "../Controller/GuitarPracticeController.h"
#include "../View/TopBar.h"
#include "../View/CenterPanel.h"
#include "../View/LeftPanel.h"
#include "../View/RightPanel.h"
#include "../ScoreComponent.h"

// 녹음 기능을 위한 전방 선언
class AudioRecorder;
class RecordingThumbnail;

class MainComponent;  // 전방 선언

// GuitarPracticeComponent - 기타 연습 기능의 View 역할
// MVC 패턴 중 View 역할만 담당하며, 비즈니스 로직은 Controller에 위임
class GuitarPracticeComponent : public juce::Component,
                               public AudioModel::Listener  // 모델 변경 감지를 위한 리스너
{
public:
    GuitarPracticeComponent(MainComponent& mainComponent);
    ~GuitarPracticeComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // AudioModel::Listener 구현 (모델 변경 감지)
    void playStateChanged(bool isPlaying) override;
    void volumeChanged(float newVolume) override;
    
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
    // View에서는 UI 관련 작업만 처리
    void updatePlaybackState(bool isNowPlaying);
    void updateVolumeDisplay(float volume);
    void togglePlayback();  // UI 이벤트 핸들러

    MainComponent& mainComponent;
    
    // 오디오 디바이스 관리자 참조
    juce::AudioDeviceManager& deviceManager;
    
    // MVC 컴포넌트들
    AudioModel audioModel;
    std::unique_ptr<GuitarPracticeController> controller;
    
    // View 컴포넌트들
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
};