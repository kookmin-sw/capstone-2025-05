#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"
#include "Controller/AudioController.h"
#include "Controller/TransportController.h"
#include "View/TopBar.h"
#include "View/CenterPanel.h"
#include "View/LeftPanel.h"
#include "View/RightPanel.h"
#include "ScoreComponent.h"
#include "TabPlayer.h"

// 녹음 기능을 위한 전방 선언
class AudioRecorder;
class RecordingThumbnail;

class MainComponent;  // 전방 선언

// PracticeSongComponent는 View 역할만 담당하도록 수정
class PracticeSongComponent : public juce::Component,
                              public AudioModel::Listener  // 모델 변경 감지를 위한 리스너 추가
{
public:
    PracticeSongComponent(MainComponent& mainComponent);
    ~PracticeSongComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // AudioModel::Listener 구현 (모델 변경 감지)
    void playStateChanged(bool isPlaying) override;
    void volumeChanged(float newVolume) override;
    
    // Access to model and controllers
    AudioModel& getAudioModel() { return audioModel; }
    TransportController& getTransportController() { return *transportController; }
    
    // 곡 선택 관련 메서드
    bool loadSong(const juce::String& songId);
    void resetParser(); // 파서 초기화 메서드
    
    // 녹음 관련 메서드
    void startRecording();
    void stopRecording();
    bool isRecording() const;
    
    // 오디오 분석 메서드
    void analyzeRecording();
    
private:
    // View에서는 UI 관련 작업만 처리
    void updatePlaybackState(bool isNowPlaying);
    void updateVolumeDisplay(float volume);
    void togglePlayback();

    MainComponent& mainComponent;
    
    // 오디오 디바이스 관리자 - 멤버 변수로 추가
    juce::AudioDeviceManager& deviceManager;
    
    // MVC components
    AudioModel audioModel;
    std::unique_ptr<AudioController> audioController;
    std::unique_ptr<TransportController> transportController;
    
    // View components
    std::unique_ptr<TopBar> topBar;
    std::unique_ptr<CenterPanel> centerPanel;
    std::unique_ptr<LeftPanel> leftPanel;
    std::unique_ptr<RightPanel> rightPanel;
    
    // parser를 포인터로 변경하여 필요할 때만 생성
    std::unique_ptr<gp_parser::Parser> parserPtr;
    TabPlayer player;
    std::unique_ptr<ScoreComponent> scoreComponent;

    // 녹음 관련 컴포넌트
    std::unique_ptr<RecordingThumbnail> recordingThumbnail;
    std::unique_ptr<AudioRecorder> audioRecorder;
    juce::AudioFormatManager formatManager;
    juce::TextButton recordButton;
    juce::TextButton analyzeButton;
    juce::File lastRecording;
    
    juce::TextButton playButton;
    juce::MidiBuffer midiBuffer;
    
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
};