#include "GuitarPracticeComponent.h"
#include "MainComponent.h"
#include "Controller/GuitarPracticeController.h"

#include "TopBar.h"
#include "CenterPanel.h"
#include "LeftPanel.h"
#include "RightPanel.h"
#include "ScoreComponent.h"

#include "Model/RecordingThumbnail.h"
#include "Model/AudioRecorder.h"
#include "Util/EnvLoader.h"
#include "Event/Event.h"
#include "Event/EventBus.h"

GuitarPracticeComponent::GuitarPracticeComponent(MainComponent &mainComp)
    : mainComponent(mainComp), deviceManager(mainComp.getDeviceManager())
{
    // 새 인터페이스를 사용하여 모델에 등록
    audioModel.addListener(this);
    
    // 컨트롤러 초기화 
    controller = std::make_unique<GuitarPracticeController>(audioModel, deviceManager);
    controller->setView(this);
    
    // AudioController 초기화 - 오디오 입력 레벨 변경 감지용
    audioController = std::make_unique<AudioController>(audioModel, deviceManager);
    
    // 이벤트 버스에 구독 등록
    // 특정 이벤트 타입에만 관심이 있는 경우
    EventBus::getInstance().subscribe(Event::Type::AnalysisComplete, this);
    EventBus::getInstance().subscribe(Event::Type::AnalysisFailed, this);
    EventBus::getInstance().subscribe(Event::Type::SongLoaded, this);
    EventBus::getInstance().subscribe(Event::Type::SongLoadFailed, this);
    
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
    
    // View 컴포넌트 초기화 (직접 참조)
    topBar = std::make_unique<TopBar>(*this);
    centerPanel = std::make_unique<CenterPanel>();
    leftPanel = std::make_unique<LeftPanel>(audioModel); 
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
    
    // 이벤트 버스에서 구독 해제
    EventBus::getInstance().unsubscribeFromAll(this);
    
    // 녹음기 제거
    if (audioRecorder)
        deviceManager.removeAudioCallback(audioRecorder.get());
    
    // Controller는 소멸자에서 자동으로 정리됨
    audioController.reset();
    deviceManager.closeAudioDevice();
}

void GuitarPracticeComponent::paint(juce::Graphics &g)
{
    // 배경색 설정
    g.fillAll(MapleTheme::getBackgroundColour());
    
    // 컴포넌트 경계를 표시하는 것은 생략
}

void GuitarPracticeComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // 상단 바 (백 버튼, 제목 등)
    auto topBarHeight = 60;
    topBar->setBounds(bounds.removeFromTop(topBarHeight));
    
    // 하단 컨트롤 패널 (재생, 녹음, 분석 버튼 등)
    auto controlsHeight = 80;
    auto controlsArea = bounds.removeFromBottom(controlsHeight);
    
    // 버튼 및 컨트롤 크기 설정
    auto buttonWidth = 120;
    auto buttonHeight = 40;
    auto labelWidth = 100;
    auto spacing = 10;
    
    // 버튼 중앙 정렬을 위한 계산
    auto totalControlsWidth = (buttonWidth * 3) + labelWidth + (spacing * 3);
    auto startX = (controlsArea.getWidth() - totalControlsWidth) / 2;
    auto startY = (controlsArea.getHeight() - buttonHeight) / 2;
    
    // 버튼 배치
    playButton.setBounds(controlsArea.getX() + startX, 
                         controlsArea.getY() + startY, 
                         buttonWidth, buttonHeight);
    
    positionLabel.setBounds(playButton.getRight() + spacing, 
                            controlsArea.getY() + startY, 
                            labelWidth, buttonHeight);
    
    recordButton.setBounds(positionLabel.getRight() + spacing, 
                          controlsArea.getY() + startY, 
                          buttonWidth, buttonHeight);
    
    analyzeButton.setBounds(recordButton.getRight() + spacing, 
                           controlsArea.getY() + startY, 
                           buttonWidth, buttonHeight);
    
    // 녹음된 오디오 파형 표시 영역
    auto waveformHeight = 80;
    auto waveformArea = bounds.removeFromBottom(waveformHeight);
    recordingThumbnail->setBounds(waveformArea.reduced(10));
    
    // 사이드 패널 - 좌우 패널 모두 왼쪽에 배치하여 악보 표시 영역 확보
    auto sideWidth = 250;
    auto leftArea = bounds.removeFromLeft(sideWidth);
    
    // 왼쪽 패널 (입력 레벨, 볼륨 컨트롤 등)
    auto leftPanelHeight = leftArea.getHeight() / 2;
    leftPanel->setBounds(leftArea.removeFromTop(leftPanelHeight));
    
    // 오른쪽 패널 (메트로놈, 튜너 등)
    rightPanel->setBounds(leftArea);
    
    // 악보 표시 영역 - 남은 모든 공간 사용
    if (scoreComponent)
    {
        DBG("Setting ScoreComponent bounds: " + juce::String(bounds.getWidth()) + "x" + juce::String(bounds.getHeight()));
        scoreComponent->setBounds(bounds.reduced(10));
        scoreComponent->toFront(false);
    }
    
    // 센터 패널은 더 이상 사용하지 않거나 필요한 경우에만 사용
    // centerPanel->setBounds(bounds.reduced(10));
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

void GuitarPracticeComponent::onInputLevelChanged(float newLevel)
{
    // 필요한 경우 여기서 입력 레벨 UI를 업데이트합니다
    // 현재는 leftPanel에서 처리하므로 특별한 동작이 필요 없습니다
}

// UI 업데이트 메서드
void GuitarPracticeComponent::updatePlaybackState(bool isNowPlaying)
{
    // 재생 버튼 텍스트 업데이트
    playButton.setButtonText(isNowPlaying ? "Pause" : "Play");
    
    // ScoreComponent 업데이트
    if (scoreComponent != nullptr)
    {
        if (isNowPlaying)
            scoreComponent->startPlayback();
        else
            scoreComponent->stopPlayback();
    }
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
    // 스코어 컴포넌트 업데이트 (필요시)
    if (scoreComponent != nullptr) {
        try {
            scoreComponent->updateScore();
        }
        catch (const std::exception& e) {
            DBG("Error updating score component: " + juce::String(e.what()));
        }
    }
}

void GuitarPracticeComponent::startRecording()
{
    // 재생 중이면 중지 (Controller에게 위임)
    if (audioModel.isPlaying())
        controller->togglePlayback();
    
    // 임시 녹음 파일 경로 설정
    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    lastRecording = parentDir.getNonexistentChildFile("Maple_Recording", ".wav");
    
    // 녹음 시작 (AudioRecorder 호출)
    audioRecorder->startRecording(lastRecording);
    
    // UI 업데이트
    recordButton.setButtonText("Stop");
    analyzeButton.setEnabled(false);
    
    // 썸네일 표시 모드 설정
    recordingThumbnail->setDisplayFullThumbnail(false);
}

void GuitarPracticeComponent::stopRecording()
{
    // 녹음 중지 (AudioRecorder 호출)
    audioRecorder->stop();
    
    // 썸네일 설정
    recordingThumbnail->setSource(lastRecording);
    recordingThumbnail->setDisplayFullThumbnail(true);
    
    // UI 업데이트
    recordButton.setButtonText("Record");
    analyzeButton.setEnabled(true);
}

bool GuitarPracticeComponent::isRecording() const
{
    // AudioRecorder의 상태 확인
    return audioRecorder != nullptr && audioRecorder->isRecording();
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

// IEventListener 인터페이스 구현
bool GuitarPracticeComponent::onEvent(const Event& event)
{
    // 이벤트 타입에 따라 처리
    switch (event.getType())
    {
        case Event::Type::AnalysisComplete:
            handleAnalysisCompleteEvent(static_cast<const AnalysisCompleteEvent&>(event));
            return true;
            
        case Event::Type::AnalysisFailed:
            handleAnalysisFailedEvent(static_cast<const AnalysisFailedEvent&>(event));
            return true;
            
        case Event::Type::SongLoaded:
            handleSongLoadedEvent(static_cast<const SongLoadedEvent&>(event));
            return true;
            
        case Event::Type::SongLoadFailed:
            handleSongLoadFailedEvent(static_cast<const SongLoadFailedEvent&>(event));
            return true;
            
        default:
            // 처리하지 않는 이벤트 타입
            return false;
    }
}

// 이벤트 핸들러 메서드들
void GuitarPracticeComponent::handleAnalysisCompleteEvent(const AnalysisCompleteEvent& event)
{
    DBG("GuitarPracticeComponent: Handling AnalysisCompleteEvent");
    
    const juce::var& result = event.getResult();
    
    // 분석 결과를 UI에 표시
    if (result.hasProperty("feedback") && result["feedback"].hasProperty("messages"))
    {
        juce::String feedbackText;
        const auto& messages = result["feedback"]["messages"];
        
        if (auto* array = messages.getArray())
        {
            // 배열에서 메시지 추출
            for (const auto& msg : *array)
            {
                if (msg.hasProperty("text"))
                    feedbackText += "• " + msg["text"].toString() + "\n";
            }
        }
        
        // 메시지가 있으면 다이얼로그로 표시
        if (feedbackText.isNotEmpty())
        {
            juce::MessageManager::callAsync([this, text = feedbackText]() {
                auto* dialog = new juce::DialogWindow("Analysis Feedback", 
                                                     juce::Colours::lightgrey,
                                                     true, true);
                                                     
                auto* textEditor = new juce::TextEditor();
                textEditor->setMultiLine(true);
                textEditor->setReadOnly(true);
                textEditor->setText(text);
                textEditor->setSize(400, 300);
                
                dialog->setContentOwned(textEditor, true);
                dialog->centreWithSize(450, 350);
                dialog->setVisible(true);
            });
        }
        else
        {
            // 피드백이 없는 경우 간단한 알림만 표시
            juce::MessageManager::callAsync([this]() {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Analysis Complete",
                    "The analysis has been completed. No specific feedback available.",
                    "OK"
                );
            });
        }
    }
    else
    {
        // 피드백 구조가 없는 경우 간단한 알림만 표시
        juce::MessageManager::callAsync([this]() {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Analysis Complete",
                "The analysis has been completed successfully.",
                "OK"
            );
        });
    }
    
    // 결과를 자세히 로깅 (디버깅용)
    DBG("Analysis result JSON: " + juce::JSON::toString(result));
}

void GuitarPracticeComponent::handleAnalysisFailedEvent(const AnalysisFailedEvent& event)
{
    DBG("GuitarPracticeComponent: Handling AnalysisFailedEvent");
    
    // 실패 메시지를 UI에 표시
    juce::String errorMessage = event.getErrorMessage();
    
    juce::MessageManager::callAsync([this, error = errorMessage]() {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Analysis Failed",
            error,
            "OK"
        );
    });
}

void GuitarPracticeComponent::handleSongLoadedEvent(const SongLoadedEvent& event)
{
    DBG("GuitarPracticeComponent: Handling SongLoadedEvent for song: " + event.getSongId());
    
    // UI 업데이트
    juce::MessageManager::callAsync([this]() {
        playButton.setEnabled(true);
        recordButton.setEnabled(true);
        
        // ScoreComponent 업데이트
        if (scoreComponent != nullptr) {
            try {
                scoreComponent->updateScore();
                
                // 포커스 설정으로 키보드 내비게이션 활성화
                scoreComponent->grabKeyboardFocus();
                
                // 스코어 컴포넌트를 최상위로 가져오기
                scoreComponent->toFront(false);
                
                DBG("Score component updated successfully");
            }
            catch (const std::exception& e) {
                DBG("Error updating score component: " + juce::String(e.what()));
            }
        }
    });
}

void GuitarPracticeComponent::handleSongLoadFailedEvent(const SongLoadFailedEvent& event)
{
    DBG("GuitarPracticeComponent: Handling SongLoadFailedEvent");
    
    // 실패 메시지를 UI에 표시
    juce::String songId = event.getSongId();
    juce::String errorMessage = event.getErrorMessage();
    
    juce::MessageManager::callAsync([this, song = songId, error = errorMessage]() {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Failed to Load Song",
            "Could not load song ID: " + song + "\nError: " + error,
            "OK"
        );
    });
}