#include "GuitarPracticeComponent.h"
#include "MainComponent.h"
#include "Controller/GuitarPracticeController.h"

#include "View/TopBar.h"
#include "View/CenterPanel.h"
#include "View/LeftPanel.h"
#include "View/RightPanel.h"

#include "EnvLoader.h"

// 녹음 관련 클래스들은 그대로 유지 (리팩토링 2단계에서 별도 파일로 이동 예정)
class RecordingThumbnail : public juce::Component,
                          private juce::ChangeListener
{
    // ...existing code...
};

class AudioRecorder : public juce::AudioIODeviceCallback
{
    // ...existing code...
};

GuitarPracticeComponent::GuitarPracticeComponent(MainComponent &mainComp)
    : mainComponent(mainComp), deviceManager(mainComp.getDeviceManager())
{
    // 새 인터페이스를 사용하여 모델에 등록
    audioModel.addListener(this);
    
    controller = std::make_unique<GuitarPracticeController>(audioModel, deviceManager);
    controller->setView(this);
    
    // 재생 버튼 초기화
    playButton.setButtonText("Play");
    playButton.onClick = [this]() { togglePlayback(); };
    addAndMakeVisible(playButton);
    playButton.setEnabled(false); // 곡이 선택되기 전까지 비활성화
    
    // 재생 위치 표시 레이블 초기화
    positionLabel.setText("00:00", juce::dontSendNotification);
    positionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(positionLabel);
    
    // 녹음 관련 컴포넌트 초기화 
    formatManager.registerBasicFormats();
    recordingThumbnail = std::make_unique<RecordingThumbnail>();
    audioRecorder = std::make_unique<AudioRecorder>(recordingThumbnail->getAudioThumbnail());
    
    // 녹음 버튼 초기화
    recordButton.setButtonText("Record");
    recordButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff5c5c));
    recordButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    recordButton.onClick = [this]() {
        if (isRecording())
            stopRecording();
        else
            startRecording();
    };
    addAndMakeVisible(recordButton);
    recordButton.setEnabled(false); // 곡이 선택되기 전까지 비활성화
    
    // 분석 버튼 초기화
    analyzeButton.setButtonText("Analyze");
    analyzeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff5c5cff));
    analyzeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    analyzeButton.onClick = [this]() { analyzeRecording(); };
    addAndMakeVisible(analyzeButton);
    analyzeButton.setEnabled(false); // 녹음된 파일이 없으면 비활성화

    // AudioRecorder를 오디오 콜백으로 등록
    deviceManager.addAudioCallback(audioRecorder.get());
    
    // View 컴포넌트 초기화
    topBar = std::make_unique<TopBar>(*this);
    centerPanel = std::make_unique<CenterPanel>();
    leftPanel = std::make_unique<LeftPanel>(audioModel); // Model 전달 (추후 인터페이스로 변경 예정)
    rightPanel = std::make_unique<RightPanel>();
    
    try {
        // ScoreComponent는 controller의 TabPlayer를 사용하도록 수정
        scoreComponent = std::make_unique<ScoreComponent>(controller->getTabPlayer());
        addAndMakeVisible(scoreComponent.get());
    }
    catch (const std::exception& e) {
        DBG("Error creating ScoreComponent: " + juce::String(e.what()));
        // 예외가 발생해도 계속 진행
    }

    // 컴포넌트 추가
    addAndMakeVisible(topBar.get());
    addAndMakeVisible(centerPanel.get());
    addAndMakeVisible(leftPanel.get());
    addAndMakeVisible(rightPanel.get());
    addAndMakeVisible(recordingThumbnail.get());
}

GuitarPracticeComponent::~GuitarPracticeComponent()
{
    // 모델에서 리스너 제거 (새 인터페이스 사용)
    audioModel.removeListener(this);
    
    // 녹음기 제거
    if (audioRecorder)
        deviceManager.removeAudioCallback(audioRecorder.get());
    
    // Controller는 소멸자에서 자동으로 정리됨
}

void GuitarPracticeComponent::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void GuitarPracticeComponent::resized()
{
    auto bounds = getLocalBounds();
    auto topBarHeight = 50;
    auto controlsHeight = 40;
    
    // 상단 바
    topBar->setBounds(bounds.removeFromTop(topBarHeight));
    
    // 하단에 악보 표시 영역
    auto scoreHeight = 300;
    auto scoreArea = bounds.removeFromBottom(scoreHeight);
    if (scoreComponent)
        scoreComponent->setBounds(scoreArea);
    
    // 하단 컨트롤 영역 (재생, 녹음, 분석 버튼, 위치 레이블)
    auto controlsArea = bounds.removeFromBottom(controlsHeight);
    auto buttonWidth = 120;
    auto labelWidth = 100;
    auto spacing = 10;
    
    playButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    controlsArea.removeFromLeft(spacing);
    positionLabel.setBounds(controlsArea.removeFromLeft(labelWidth));
    controlsArea.removeFromLeft(spacing);
    recordButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    controlsArea.removeFromLeft(spacing);
    analyzeButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    
    // 녹음된 오디오 파형 표시 영역
    auto waveformHeight = 100;
    auto waveformArea = bounds.removeFromBottom(waveformHeight);
    recordingThumbnail->setBounds(waveformArea);
    
    // 좌우 패널
    leftPanel->setBounds(bounds.removeFromLeft(300));
    rightPanel->setBounds(bounds.removeFromRight(300));
    
    // 중앙 패널
    centerPanel->setBounds(bounds.reduced(50));
}

// 새로운 인터페이스 메서드 구현
void GuitarPracticeComponent::onPlayStateChanged(bool isPlaying)
{
    // UI 업데이트
    updatePlaybackState(isPlaying);
}

void GuitarPracticeComponent::onVolumeChanged(float newVolume)
{
    // UI 업데이트
    updateVolumeDisplay(newVolume);
}

void GuitarPracticeComponent::onPositionChanged(double positionInSeconds)
{
    // UI 업데이트
    updatePositionDisplay(positionInSeconds);
}

// UI 업데이트 메서드
void GuitarPracticeComponent::updatePlaybackState(bool isNowPlaying)
{
    playButton.setButtonText(isNowPlaying ? "Stop" : "Play");
}

void GuitarPracticeComponent::updateVolumeDisplay(float volume)
{
    // 볼륨 표시 UI 요소가 있을 경우 여기서 업데이트
}

void GuitarPracticeComponent::updatePositionDisplay(double positionInSeconds)
{
    // 시간 포맷: mm:ss
    int minutes = static_cast<int>(positionInSeconds) / 60;
    int seconds = static_cast<int>(positionInSeconds) % 60;
    positionLabel.setText(juce::String::formatted("%02d:%02d", minutes, seconds), juce::dontSendNotification);
}

void GuitarPracticeComponent::togglePlayback()
{
    // Controller에게 로직 위임
    controller->togglePlayback();
}

// UI에서 호출되는 메서드 - Controller에게 위임
void GuitarPracticeComponent::loadSong(const juce::String& songId)
{
    bool success = controller->loadSong(songId);
    
    // UI 업데이트 (View의 책임)
    if (success) {
        playButton.setEnabled(true);
        recordButton.setEnabled(true);
        
        // ScoreComponent 업데이트
        if (scoreComponent != nullptr) {
            try {
                scoreComponent->updateScore();
            }
            catch (const std::exception& e) {
                DBG("Error updating score component: " + juce::String(e.what()));
            }
        }
    }
}

// UI 업데이트 메서드
void GuitarPracticeComponent::updateUI()
{
    // 향후 추가 UI 업데이트 필요시 구현
}

void GuitarPracticeComponent::startRecording()
{
    // 재생 중이면 중지 (Controller에게 위임)
    if (audioModel.isPlaying())
        controller->togglePlayback();
    
    // 임시 녹음 파일 경로 설정
    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    lastRecording = parentDir.getNonexistentChildFile("Recording", ".wav");
    
    // 녹음 시작 (View에서 직접 처리 - AudioRecorder 컴포넌트는 아직 View에 남아있음)
    audioRecorder->startRecording(lastRecording);
    
    // UI 업데이트
    recordButton.setButtonText("Stop");
    analyzeButton.setEnabled(false);
    
    DBG("Recording started: " + lastRecording.getFullPathName());
}

void GuitarPracticeComponent::stopRecording()
{
    // 녹음 중지 (View에서 직접 처리)
    audioRecorder->stop();
    
    // UI 업데이트
    recordButton.setButtonText("Record");
    analyzeButton.setEnabled(true);
    
    DBG("Recording finished: " + lastRecording.getFullPathName());
}

bool GuitarPracticeComponent::isRecording() const
{
    return audioRecorder && audioRecorder->isRecording();
}

void GuitarPracticeComponent::analyzeRecording()
{
    // Controller에게 분석 작업 위임
    if (lastRecording.existsAsFile()) {
        controller->analyzeRecording(lastRecording);
    } else {
        DBG("No recorded file to analyze.");
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                              "Analysis Error",
                                              "No recording file found. Please record audio first.",
                                              "OK");
    }
}