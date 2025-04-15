#pragma once
#include <JuceHeader.h>
#include "Model/AudioModel.h"
#include "Controller/AudioController.h"
#include "Controller/TransportController.h"
#include "View/TopBar.h"
#include "View/CenterPanel.h"
#include "View/LeftPanel.h"
#include "View/RightPanel.h"
#include "View/EffectControls.h"
#include "View/WaveformGraph.h"
#include "AudioPlaying.h"
#include "ScoreComponent.h"
#include "TabPlayer.h"

// 녹음 기능을 위한 전방 선언
class AudioRecorder;
class RecordingThumbnail;

class MainComponent;  // 전방 선언

class PracticeSongComponent : public juce::Component,
                              public juce::AudioIODeviceCallback
{
public:
    PracticeSongComponent(MainComponent& mainComponent);
    ~PracticeSongComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Transport-related methods
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    
    // AudioIODeviceCallback 구현
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                         float* const* outputChannelData, int numOutputChannels,
                                         int numSamples, const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    
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
    void togglePlayback();

    MainComponent& mainComponent;
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
    std::unique_ptr<EffectControls> effectControls;
    // std::unique_ptr<AudioPlaybackDemo> waveformGraph;
    
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
    bool isPlaying = false;
    
    juce::AudioTransportSource transport;
    double sampleRate = 0.0;
    
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