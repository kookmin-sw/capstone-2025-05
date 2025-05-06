#include "GuitarPracticeComponent.h"
#include "MainComponent.h"
#include "Controller/GuitarPracticeController.h"
#include "Controller/AudioController.h"

#include "TopBar.h"
#include "CenterPanel.h"
#include "LeftPanel.h"
#include "RightPanel.h"
#include "ScoreComponent.h"
#include "PracticeSettingsComponent.h"
#include "PerformanceAnalysisComponent.h"
#include "FingeringGuideComponent.h"
#include "PracticeProgressComponent.h"

#include "Model/RecordingThumbnail.h"
#include "Model/AudioRecorder.h"
#include "Util/EnvLoader.h"
#include "Event/Event.h"
#include "Event/EventBus.h"

// 커스텀 LookAndFeel 클래스 포함 - 반드시 개별 헤더 파일로 정의된 클래스들만 포함
#include "LookAndFeel/MaplePlayButton.h"
#include "LookAndFeel/MapleRecordButton.h"
#include "LookAndFeel/MapleAnalyzeButton.h"

GuitarPracticeComponent::GuitarPracticeComponent(MainComponent &mainComp)
    : mainComponent(mainComp), deviceManager(mainComp.getDeviceManager())
{
    DBG("GuitarPracticeComponent - constructor start");
    
    // 오디오 모델 초기화 및 리스너 등록 (새 인터페이스 사용)
    audioModel.addListener(this);
    
    // 모델 초기 상태 업데이트
    updateUI();
    
    // AudioController 초기화
    audioController = std::make_unique<AudioController>(audioModel, deviceManager);
    
    // GuitarPracticeController 초기화 (파라미터 추가)
    controller = std::make_unique<GuitarPracticeController>(audioModel, deviceManager);
    
    // AudioController에 GuitarPracticeController 설정 (음원 재생 연결)
    audioController->setGuitarPracticeController(controller.get());
    
    // 오디오 콜백 등록 (컨트롤러를 콜백으로 추가하여 음원 재생 가능)
    deviceManager.addAudioCallback(audioController.get());
    
    // 이벤트 버스에 리스너로 등록
    EventBus::getInstance().subscribe(Event::Type::AnalysisComplete, this);
    EventBus::getInstance().subscribe(Event::Type::AnalysisFailed, this);
    EventBus::getInstance().subscribe(Event::Type::SongLoaded, this);
    EventBus::getInstance().subscribe(Event::Type::SongLoadFailed, this);
    
    // 커스텀 LookAndFeel 인스턴스 생성
    recordButtonLookAndFeel = std::make_unique<MapleRecordButton>();
    analyzeButtonLookAndFeel = std::make_unique<MapleAnalyzeButton>();
    playButtonLookAndFeel = std::make_unique<MaplePlayButton>(); // Play 버튼 LookAndFeel 추가
    
    // UI 컴포넌트 초기화
    playButton.setButtonText("Play");
    playButton.onClick = [this]() { togglePlayback(); };
    playButton.setToggleState(false, juce::dontSendNotification);
    playButton.setLookAndFeel(playButtonLookAndFeel.get());
    addAndMakeVisible(playButton);
    
    positionLabel.setText("00:00", juce::dontSendNotification);
    positionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(positionLabel);
    
    // 마이크 모니터링 버튼 초기화
    micMonitorButton.setButtonText("Monitor");
    micMonitorButton.setToggleState(false, juce::dontSendNotification);
    micMonitorButton.onClick = [this]() { toggleMicrophoneMonitoring(); };
    micMonitorButton.setColour(juce::ToggleButton::tickColourId, MapleTheme::getAccentColour());
    micMonitorButton.setColour(juce::ToggleButton::tickDisabledColourId, MapleTheme::getAccentColour().withAlpha(0.5f));
    micMonitorButton.setColour(juce::ToggleButton::textColourId, MapleTheme::getHighlightColour());
    addAndMakeVisible(micMonitorButton);
    
    // 마이크 게인 슬라이더 초기화
    micGainLabel.setText("Gain", juce::dontSendNotification);
    micGainLabel.setJustificationType(juce::Justification::centredRight);
    micGainLabel.setColour(juce::Label::textColourId, MapleTheme::getTextColour());
    addAndMakeVisible(micGainLabel);

    micGainSlider.setRange(0.1, 10.0, 0.1);
    micGainSlider.setValue(5.0); // 기본값
    micGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    micGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    micGainSlider.setColour(juce::Slider::thumbColourId, MapleTheme::getAccentColour());
    micGainSlider.setColour(juce::Slider::trackColourId, MapleTheme::getAccentColour().withAlpha(0.6f));
    micGainSlider.addListener(this);
    addAndMakeVisible(micGainSlider);
    
    // 뷰 전환 버튼 초기화
    analysisViewButton.setButtonText("Performance Analysis");
    analysisViewButton.onClick = [this]() { switchBottomView(BottomViewType::PerformanceAnalysis); };
    analysisViewButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(analysisViewButton);
    
    fingeringViewButton.setButtonText("Fingering Guide");
    fingeringViewButton.onClick = [this]() { switchBottomView(BottomViewType::FingeringGuide); };
    fingeringViewButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(fingeringViewButton);
    
    progressViewButton.setButtonText("Practice Progress");
    progressViewButton.onClick = [this]() { switchBottomView(BottomViewType::PracticeProgress); };
    progressViewButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(progressViewButton);
    
    // AmpliTube 에디터 버튼 초기화
    ampliTubeViewButton.setButtonText("AmpliTube");
    ampliTubeViewButton.onClick = [this]() { switchBottomView(BottomViewType::AmpliTubeEditor); };
    ampliTubeViewButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(ampliTubeViewButton);
    
    // AmpliTube 활성화 버튼 초기화
    ampliTubeEnableButton.setButtonText("Enable AmpliTube");
    ampliTubeEnableButton.setToggleState(false, juce::dontSendNotification);
    ampliTubeEnableButton.onClick = [this]() { toggleAmpliTubeEffect(); };
    ampliTubeEnableButton.setColour(juce::ToggleButton::tickColourId, MapleTheme::getAccentColour());
    ampliTubeEnableButton.setColour(juce::ToggleButton::textColourId, MapleTheme::getHighlightColour());
    
    // 오디오 포맷 매니저 초기화
    formatManager.registerBasicFormats();
    
    recordingThumbnail = std::make_unique<RecordingThumbnail>();
    audioRecorder = std::make_unique<AudioRecorder>(recordingThumbnail->getAudioThumbnail());
    
    // 녹음 버튼 초기화 - 원형 버튼으로 개선
    recordButton.setButtonText("");
    recordButton.onClick = [this]() {
        if (isRecording())
            stopRecording();
        else
            startRecording();
    };
    recordButton.setEnabled(false); // 곡이 선택되기 전까지 비활성화
    recordButton.setLookAndFeel(recordButtonLookAndFeel.get());
    addAndMakeVisible(recordButton);

    // 분석 버튼 초기화 - 원형 버튼으로 개선 
    analyzeButton.setButtonText("");
    analyzeButton.onClick = [this]() { analyzeRecording(); };
    analyzeButton.setEnabled(false); // 녹음된 파일이 없으면 비활성화
    analyzeButton.setLookAndFeel(analyzeButtonLookAndFeel.get());
    addAndMakeVisible(analyzeButton);
    
    // AudioRecorder를 오디오 콜백으로 등록
    deviceManager.addAudioCallback(audioRecorder.get());
    
    // View 컴포넌트 초기화 (직접 참조)
    topBar = std::make_unique<TopBar>(*this);
    centerPanel = std::make_unique<CenterPanel>();
    leftPanel = std::make_unique<LeftPanel>(audioModel); 
    rightPanel = std::make_unique<RightPanel>();
    
    // 연습 설정 컴포넌트 초기화
    practiceSettingsComponent = std::make_unique<PracticeSettingsComponent>();
    practiceSettingsComponent->setOnSettingsChanged([this]() {
        // 설정 변경 시 TabPlayer에 적용하는 코드
        if (controller != nullptr) {
            // 템포, 루프 설정, 재생 속도 등을 적용
            // TODO: TabPlayer에서 이러한 설정을 적용하는 메서드 구현 필요
            DBG("Practice settings changed: Tempo=" + juce::String(practiceSettingsComponent->getTempo()));
        }
    });
    addAndMakeVisible(practiceSettingsComponent.get());
    
    try {
        // ScoreComponent는 controller의 TabPlayer를 사용하도록 수정
        scoreComponent = std::make_unique<ScoreComponent>(controller->getTabPlayer());
        // PlaybackStateChangeListener 등록
        scoreComponent->setPlaybackStateChangeListener(this);
        // 컴포넌트가 키보드 포커스를 받을 수 있도록 설정
        scoreComponent->setWantsKeyboardFocus(true);
        addAndMakeVisible(scoreComponent.get());
    }
    catch (const std::exception& e) {
        DBG("Error creating ScoreComponent: " + juce::String(e.what()));
        // 예외가 발생해도 계속 진행
    }
    
    // 새로운 컴포넌트 초기화
    performanceAnalysisComponent = std::make_unique<PerformanceAnalysisComponent>();
    fingeringGuideComponent = std::make_unique<FingeringGuideComponent>();
    practiceProgressComponent = std::make_unique<PracticeProgressComponent>();
    
    // AmpliTube 에디터 컨테이너 초기화
    ampliTubeEditorContainer = std::make_unique<juce::Component>();
    ampliTubeEditorContainer->addAndMakeVisible(ampliTubeEnableButton);
    
    addAndMakeVisible(performanceAnalysisComponent.get());
    addChildComponent(fingeringGuideComponent.get());
    addChildComponent(practiceProgressComponent.get());
    addChildComponent(ampliTubeEditorContainer.get());

    // 컴포넌트 추가
    addAndMakeVisible(topBar.get());
    addAndMakeVisible(centerPanel.get());
    addAndMakeVisible(leftPanel.get());
    addAndMakeVisible(rightPanel.get());
    addAndMakeVisible(recordingThumbnail.get());
    
    // 전체 컴포넌트가 키보드 포커스를 받을 수 있도록 설정
    setWantsKeyboardFocus(true);
    
    // 현재 표시할 하단 뷰 설정
    currentBottomView = BottomViewType::PerformanceAnalysis;
    updateBottomViewVisibility();
    
    // 버튼 상태 업데이트
    updateViewButtonStates();
    
    DBG("GuitarPracticeComponent - constructor complete");
}

GuitarPracticeComponent::~GuitarPracticeComponent()
{
    // 타이머 정리
    stopTimer();

    // 모델에서 리스너 제거 (새 인터페이스 사용)
    audioModel.removeListener(this);
    
    // 이벤트 버스에서 구독 해제
    EventBus::getInstance().unsubscribeFromAll(this);
    
    // 녹음기 제거
    if (audioRecorder)
        deviceManager.removeAudioCallback(audioRecorder.get());
    
    // 오디오 컨트롤러 제거
    if (audioController)
        deviceManager.removeAudioCallback(audioController.get());
    
    // LookAndFeel 정리
    recordButton.setLookAndFeel(nullptr);
    analyzeButton.setLookAndFeel(nullptr);
    playButton.setLookAndFeel(nullptr);
    
    // Controller는 소멸자에서 자동으로 정리됨
    audioController.reset();
    deviceManager.closeAudioDevice();
}

void GuitarPracticeComponent::paint(juce::Graphics &g)
{
    // 배경색 설정
    g.fillAll(MapleTheme::getBackgroundColour());
    
    // 컨트롤 패널 배경
    auto bounds = getLocalBounds();
    auto topBarHeight = 60;
    bounds.removeFromTop(topBarHeight);
    
    auto controlsHeight = 80;
    auto controlsArea = bounds.removeFromBottom(controlsHeight);
    g.setColour(MapleTheme::getCardColour());
    g.fillRoundedRectangle(controlsArea.toFloat().reduced(5.0f), 8.0f);
    
    auto waveformHeight = 80;
    auto waveformArea = bounds.removeFromBottom(waveformHeight);
    g.setColour(MapleTheme::getCardColour().darker(0.05f));
    g.fillRoundedRectangle(waveformArea.toFloat().reduced(5.0f), 8.0f);
}

void GuitarPracticeComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // 상단 바 (백 버튼, 제목 등)
    auto topBarHeight = 60;
    topBar->setBounds(bounds.removeFromTop(topBarHeight));
    
    // 하단 컨트롤 패널 (재생, 녹음, 분석 버튼 등) - 약간의 마진 추가
    auto controlsHeight = 80;
    auto controlsArea = bounds.removeFromBottom(controlsHeight).reduced(10, 5);
    
    // 버튼 및 컨트롤 크기 설정 - 원형 버튼으로 변경
    auto buttonSize = 50; // 원형 버튼 크기
    auto buttonWidth = 120; // 일반 버튼 너비 
    auto buttonHeight = 45;
    auto labelWidth = 100;
    auto spacing = 15;
    
    // 버튼 중앙 정렬을 위한 계산
    auto totalControlsWidth = buttonWidth + (buttonSize * 2) + labelWidth + (spacing * 5) + 100 + 40 + 120; // 모니터링 버튼, 게인 라벨 및 슬라이더 포함
    auto startX = (controlsArea.getWidth() - totalControlsWidth) / 2;
    auto startY = (controlsArea.getHeight() - buttonHeight) / 2;
    
    // 버튼 배치 - 둥근 모서리로 스타일 개선
    playButton.setBounds(controlsArea.getX() + startX, 
                         controlsArea.getY() + startY, 
                         buttonWidth, buttonHeight);
    
    positionLabel.setBounds(playButton.getRight() + spacing, 
                            controlsArea.getY() + startY, 
                            labelWidth, buttonHeight);
    
    // 녹음/분석 버튼은 원형으로 설정
    recordButton.setBounds(positionLabel.getRight() + spacing, 
                          controlsArea.getY() + startY - (buttonSize - buttonHeight)/2, 
                          buttonSize, buttonSize);
    
    analyzeButton.setBounds(recordButton.getRight() + spacing, 
                           controlsArea.getY() + startY - (buttonSize - buttonHeight)/2, 
                           buttonSize, buttonSize);
    
    // 마이크 모니터링 버튼 배치
    micMonitorButton.setBounds(analyzeButton.getRight() + spacing,
                             controlsArea.getY() + startY,
                             100, buttonHeight);
    
    // 마이크 게인 슬라이더 배치
    micGainLabel.setBounds(micMonitorButton.getRight() + spacing,
                          controlsArea.getY() + startY,
                          40, buttonHeight);
    micGainSlider.setBounds(micGainLabel.getRight() + 5,
                           controlsArea.getY() + startY,
                           120, buttonHeight);
    
    // 녹음된 오디오 파형 표시 영역 - 마진 추가
    auto waveformHeight = 80;
    auto waveformArea = bounds.removeFromBottom(waveformHeight).reduced(15, 5);
    recordingThumbnail->setBounds(waveformArea);
    
    // 사이드 패널 - 왼쪽과 오른쪽에 패널 배치, 마진 추가
    auto sideWidth = 250;
    
    // 왼쪽 패널 (입력 레벨, 볼륨 컨트롤 등)
    auto leftArea = bounds.removeFromLeft(sideWidth).reduced(5, 10);
    leftPanel->setBounds(leftArea.removeFromTop(leftArea.getHeight() / 2 - 5));
    
    // 연습 설정 컴포넌트 (왼쪽 아래 배치)
    practiceSettingsComponent->setBounds(leftArea.removeFromBottom(leftArea.getHeight() - 5));
    
    // 오른쪽 패널 (메트로놈, 튜너 등)
    auto rightArea = bounds.removeFromRight(sideWidth).reduced(5, 10);
    rightPanel->setBounds(rightArea);
    
    // 악보 표시 영역 - 높이를 줄여서 하단에 새 컴포넌트를 위한 공간 확보
    if (scoreComponent)
    {
        // ScoreComponent의 실제 필요한 크기 계산
        juce::Rectangle<int> contentBounds = scoreComponent->getScoreContentBounds();
        int contentHeight = contentBounds.getHeight();
        
        // 콘텐츠 높이가 0이면 기본값 설정
        if (contentHeight <= 0) {
            contentHeight = 200; // 기본 높이
        }
        
        // 최대 높이 제한 (화면의 70%를 넘지 않도록)
        int maxHeight = bounds.getHeight() * 0.7;
        int scoreHeight = juce::jmin(contentHeight, static_cast<int>(maxHeight));
        
        // 최소 높이 보장
        scoreHeight = juce::jmax(200, scoreHeight);
        
        auto scoreArea = bounds.removeFromTop(scoreHeight).reduced(15, 10);
        DBG("Setting ScoreComponent bounds: " + juce::String(scoreArea.getWidth()) + "x" + juce::String(scoreArea.getHeight()));
        DBG("Score content size: " + juce::String(contentBounds.getWidth()) + "x" + juce::String(contentBounds.getHeight()));
        
        scoreComponent->setBounds(scoreArea);
        scoreComponent->toFront(false);
    }
    
    // 뷰 전환 버튼 배치 - 악보와 하단 뷰 사이
    auto buttonBarHeight = 30;
    auto buttonBar = bounds.removeFromTop(buttonBarHeight);
    
    // 버튼 사이즈 및 위치 계산
    auto tabButtonWidth = 120;
    auto totalTabsWidth = tabButtonWidth * 4 + spacing * 3; // 버튼 4개로 변경
    auto tabStartX = buttonBar.getCentreX() - totalTabsWidth / 2;
    
    analysisViewButton.setBounds(tabStartX, buttonBar.getY(), tabButtonWidth, buttonBarHeight);
    fingeringViewButton.setBounds(analysisViewButton.getRight() + spacing, buttonBar.getY(), tabButtonWidth, buttonBarHeight);
    progressViewButton.setBounds(fingeringViewButton.getRight() + spacing, buttonBar.getY(), tabButtonWidth, buttonBarHeight);
    ampliTubeViewButton.setBounds(progressViewButton.getRight() + spacing, buttonBar.getY(), tabButtonWidth, buttonBarHeight);
    
    // 하단 뷰 컴포넌트 배치 (악보 아래 공간)
    auto bottomViewArea = bounds.reduced(15, 10);
    performanceAnalysisComponent->setBounds(bottomViewArea);
    fingeringGuideComponent->setBounds(bottomViewArea);
    practiceProgressComponent->setBounds(bottomViewArea);
    ampliTubeEditorContainer->setBounds(bottomViewArea);
    
    // AmpliTube 활성화 버튼 배치
    auto ampliTubeToggleHeight = 30;
    auto ampliTubeToggleArea = ampliTubeEditorContainer->getLocalBounds().removeFromTop(ampliTubeToggleHeight);
    ampliTubeEnableButton.setBounds(ampliTubeToggleArea);
    
    // AmpliTube 에디터 배치 (활성화 버튼 아래)
    auto editorArea = ampliTubeEditorContainer->getLocalBounds();
    editorArea.removeFromTop(ampliTubeToggleHeight);
    
    // AmpliTube 에디터가 있으면 배치
    if (auto* editor = audioController->getAmpliTubeEditorComponent())
    {
        if (!ampliTubeEditorContainer->isParentOf(editor))
        {
            ampliTubeEditorContainer->addAndMakeVisible(editor);
        }
        editor->setBounds(editorArea);
    }
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

// PlaybackStateChangeListener 인터페이스 구현
void GuitarPracticeComponent::onPlaybackStateChanged(bool isPlaying)
{
    // TabPlayer에서 발생한 상태 변경을 AudioModel에 전달
    // 이미 같은 상태이면 불필요한 이벤트 발생 방지
    if (audioModel.isPlaying() != isPlaying)
    {
        // 오디오 모델 상태 변경
        audioModel.setPlaying(isPlaying);
        
        // 컨트롤러도 동기화
        if (isPlaying)
            controller->startPlayback();
        else
            controller->stopPlayback();
    }
    
    // UI 업데이트 - 재생 상태 변경
    updatePlaybackState(isPlaying);
}

// UI 업데이트 메서드
void GuitarPracticeComponent::updatePlaybackState(bool isNowPlaying)
{
    // 플레이 버튼의 토글 상태만 업데이트 (이벤트 발생 없이)
    if (playButton.getToggleState() != isNowPlaying)
    {
        playButton.setToggleState(isNowPlaying, juce::dontSendNotification);
        playButton.repaint(); // 확실히 UI 업데이트
    }
    
    // 재생 중일 때는 분석 버튼 비활성화
    analyzeButton.setEnabled(!isNowPlaying && lastRecording.existsAsFile());
    
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
    
    // ScoreComponent 상태와 동기화
    if (scoreComponent != nullptr)
    {
        if (audioModel.isPlaying())
            scoreComponent->startPlayback();
        else
            scoreComponent->stopPlayback();
    }
}

// UI에서 호출되는 메서드 - Controller에게 위임
void GuitarPracticeComponent::loadSong(const juce::String& songId)
{
    bool success = controller->loadSong(songId);
    
    // UI 업데이트 (View의 책임)
    if (success) {
        // 버튼 활성화
        playButton.setEnabled(true);
        recordButton.setEnabled(true);
        
        // ScoreComponent 업데이트
        if (scoreComponent != nullptr) {
            try {
                scoreComponent->updateScore();
                
                // 악보가 로드된 후 레이아웃 다시 계산 (악보 크기가 변경되었을 수 있음)
                resized();
                
                // 포커스 설정
                scoreComponent->grabKeyboardFocus();
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
    
    // UI 업데이트 - 녹음 중 상태 표시 (pulsing 효과)
    recordButtonLookAndFeel->setRecording(true);
    recordButton.repaint();
    analyzeButton.setEnabled(false);
    
    // 썸네일 표시 모드 설정
    recordingThumbnail->setDisplayFullThumbnail(false);
    
    // 녹음 중 애니메이션 시작
    startTimer(100); // 100ms 간격으로 타이머 시작
}

void GuitarPracticeComponent::stopRecording()
{
    // 녹음 중지 (AudioRecorder 호출)
    audioRecorder->stop();
    
    // 썸네일 설정
    recordingThumbnail->setSource(lastRecording);
    recordingThumbnail->setDisplayFullThumbnail(true);
    
    // UI 업데이트 - 녹음 중지 상태 표시
    recordButtonLookAndFeel->setRecording(false);
    recordButton.repaint();
    analyzeButton.setEnabled(true);
    
    // 타이머 중지
    stopTimer();
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
        // 분석 중 UI 업데이트
        analyzeButton.setEnabled(false);
        analyzeButtonLookAndFeel->setAnalyzing(true);
        analyzeButton.repaint();
        
        // 분석 중 애니메이션 시작 (이미 타이머가 동작 중이 아니라면)
        if (!isTimerRunning())
            startTimer(50); // 50ms 간격으로 타이머 시작 (더 부드러운 애니메이션)
        
        controller->analyzeRecording(lastRecording);
    } else {
        DBG("No recorded file to analyze.");
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                              "Analysis Error",
                                              "No recording file found. Please record audio first.",
                                              "OK");
    }
}

// GuitarPracticeComponent 클래스에 추가할 새 메서드
void GuitarPracticeComponent::timerCallback()
{
    // 녹음 중일 때 버튼 펄싱 효과를 위한 메서드
    if (isRecording() && recordButtonLookAndFeel) {
        recordButtonLookAndFeel->updatePulse();
        recordButton.repaint();
    }
    
    // 분석 중일 때 버튼 애니메이션 처리
    if (analyzeButtonLookAndFeel && !analyzeButton.isEnabled()) {
        analyzeButton.repaint();
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
    
    // UI 업데이트
    juce::MessageManager::callAsync([this]() {
        analyzeButton.setEnabled(true);
        analyzeButtonLookAndFeel->setAnalyzing(false);
        analyzeButton.repaint();
        
        // 녹음 중이 아니라면 타이머 중지
        if (!isRecording())
            stopTimer();
    });
    
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
                                                     MapleTheme::getCardColour(),
                                                     true, true);
                                                     
                auto* textEditor = new juce::TextEditor();
                textEditor->setMultiLine(true);
                textEditor->setReadOnly(true);
                textEditor->setText(text);
                textEditor->setColour(juce::TextEditor::backgroundColourId, MapleTheme::getCardColour());
                textEditor->setColour(juce::TextEditor::textColourId, MapleTheme::getTextColour());
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
    
    // UI 업데이트
    juce::MessageManager::callAsync([this]() {
        analyzeButton.setEnabled(true);
        analyzeButtonLookAndFeel->setAnalyzing(false);
        analyzeButton.repaint();
        
        // 녹음 중이 아니라면 타이머 중지
        if (!isRecording())
            stopTimer();
    });
    
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

// 전체 컴포넌트도 스페이스바를 처리하기 위한 메서드 추가
bool GuitarPracticeComponent::keyPressed(const juce::KeyPress& key)
{
    // 스페이스바로 재생/정지 토글
    if (key == juce::KeyPress::spaceKey)
    {
        togglePlayback();
        return true;
    }
    
    // 다른 키는 상위 컴포넌트에 위임
    return juce::Component::keyPressed(key);
}

// 새로운 메서드: 하단 뷰 가시성 업데이트
void GuitarPracticeComponent::updateBottomViewVisibility()
{
    performanceAnalysisComponent->setVisible(currentBottomView == BottomViewType::PerformanceAnalysis);
    fingeringGuideComponent->setVisible(currentBottomView == BottomViewType::FingeringGuide);
    practiceProgressComponent->setVisible(currentBottomView == BottomViewType::PracticeProgress);
    ampliTubeEditorContainer->setVisible(currentBottomView == BottomViewType::AmpliTubeEditor);
}

// 새로운 메서드: 하단 뷰 전환
void GuitarPracticeComponent::switchBottomView(BottomViewType viewType)
{
    if (currentBottomView != viewType)
    {
        currentBottomView = viewType;
        updateBottomViewVisibility();
        updateViewButtonStates();
        
        // AmpliTube 뷰로 전환시 에디터 업데이트
        if (viewType == BottomViewType::AmpliTubeEditor && audioController)
        {
            auto* editor = audioController->getAmpliTubeEditorComponent();
            if (editor && !ampliTubeEditorContainer->isParentOf(editor))
            {
                ampliTubeEditorContainer->addAndMakeVisible(editor);
                resized(); // 에디터 크기 조정
            }
        }
        
        resized(); // 레이아웃 업데이트
    }
}

// 버튼 상태 업데이트 메서드 수정
void GuitarPracticeComponent::updateViewButtonStates()
{
    analysisViewButton.setToggleState(currentBottomView == BottomViewType::PerformanceAnalysis, juce::dontSendNotification);
    fingeringViewButton.setToggleState(currentBottomView == BottomViewType::FingeringGuide, juce::dontSendNotification);
    progressViewButton.setToggleState(currentBottomView == BottomViewType::PracticeProgress, juce::dontSendNotification);
    ampliTubeViewButton.setToggleState(currentBottomView == BottomViewType::AmpliTubeEditor, juce::dontSendNotification);
}

// 마이크 모니터링 관련 메서드 개선
void GuitarPracticeComponent::enableMicrophoneMonitoring(bool shouldEnable)
{
    DBG("GuitarPracticeComponent: Setting microphone monitoring to " + juce::String(shouldEnable ? "enabled" : "disabled"));
    
    microphoneMonitoringEnabled = shouldEnable;
    
    // AudioController에 모니터링 상태 변경 전달
    if (audioController != nullptr)
    {
        audioController->enableMicrophoneMonitoring(shouldEnable);
        
        // 디버그 확인
        DBG("GuitarPracticeComponent: AudioController monitoring state = " + 
            juce::String(audioController->isMicrophoneMonitoringEnabled() ? "enabled" : "disabled"));
    }
    else
    {
        DBG("GuitarPracticeComponent: Warning - audioController is nullptr!");
    }
    
    // UI 업데이트 - 토글 상태가 변경되었을 때만 업데이트
    if (micMonitorButton.getToggleState() != shouldEnable)
    {
        micMonitorButton.setToggleState(shouldEnable, juce::dontSendNotification);
        
        // 활성화 상태에 따라 텍스트 색상 변경
        micMonitorButton.setColour(juce::ToggleButton::textColourId, 
                                  shouldEnable ? juce::Colours::green : MapleTheme::getHighlightColour());
    }
}

void GuitarPracticeComponent::toggleMicrophoneMonitoring()
{
    // 현재 상태 확인
    bool currentState = microphoneMonitoringEnabled;
    DBG("GuitarPracticeComponent: Toggle microphone monitoring from " + 
        juce::String(currentState ? "enabled" : "disabled") + " to " + 
        juce::String(!currentState ? "enabled" : "disabled"));
    
    // 현재 상태 반전
    enableMicrophoneMonitoring(!currentState);
}

// 마이크 게인 조절 메서드 구현
void GuitarPracticeComponent::setMicrophoneGain(float gain)
{
    if (audioController != nullptr)
    {
        audioController->setMicrophoneGain(gain);
        
        // UI 업데이트 (슬라이더 값이 다를 경우만)
        if (std::abs(micGainSlider.getValue() - gain) > 0.01f)
        {
            micGainSlider.setValue(gain, juce::dontSendNotification);
        }
    }
}

float GuitarPracticeComponent::getMicrophoneGain() const
{
    if (audioController != nullptr)
    {
        return audioController->getMicrophoneGain();
    }
    return 5.0f; // 기본값
}

// Slider::Listener 인터페이스 구현
void GuitarPracticeComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &micGainSlider)
    {
        float gain = static_cast<float>(micGainSlider.getValue());
        setMicrophoneGain(gain);
        
        // 현재 상태 로깅
        DBG("GuitarPracticeComponent: Microphone gain changed to " + juce::String(gain));
    }
}

// AmpliTube 이펙트 관련 메서드 구현
void GuitarPracticeComponent::enableAmpliTubeEffect(bool shouldEnable)
{
    if (audioController)
    {
        audioController->enableAmpliTubeEffect(shouldEnable);
        ampliTubeEnableButton.setToggleState(shouldEnable, juce::dontSendNotification);
        
        // 활성화 상태에 따라 텍스트 색상 변경
        ampliTubeEnableButton.setColour(juce::ToggleButton::textColourId, 
                                      shouldEnable ? juce::Colours::green : MapleTheme::getHighlightColour());
    }
}

bool GuitarPracticeComponent::isAmpliTubeEffectEnabled() const
{
    if (audioController)
        return audioController->isAmpliTubeEffectEnabled();
    return false;
}

void GuitarPracticeComponent::toggleAmpliTubeEffect()
{
    bool currentState = isAmpliTubeEffectEnabled();
    enableAmpliTubeEffect(!currentState);
}