#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(800, 400);
    
    // Settings 버튼 초기화
    settingsButton.setButtonText("Settings");
    settingsButton.addListener(this);
    addAndMakeVisible(settingsButton);
    
    // 오디오 설정 초기화
    setAudioChannels(1, 2);  // 모노 입력, 스테레오 출력
    
    // 버퍼 초기화
    audioBuffer.fill(0.0f);
    
    // 화면 갱신 타이머 시작 (60fps)
    startTimerHz(60);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &settingsButton)
    {
        auto audioSettingsComp = std::make_unique<juce::AudioDeviceSelectorComponent>(
            deviceManager,
            0, 2,  // 입력 채널
            0, 2,  // 출력 채널
            true,  // 오디오 입력 표시
            true,  // 오디오 출력 표시
            false, // 채널 설정 제외
            false  // MIDI 제외
        );

        audioSettingsComp->setSize(500, 400);

        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(audioSettingsComp.release());
        options.dialogTitle = "Audio Settings";
        options.dialogBackgroundColour = juce::Colours::darkgrey;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = false;

        settingsWindow = std::shared_ptr<juce::DialogWindow>(options.launchAsync());
    }
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // 오디오 시작 시 필요한 초기화
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // 입력 채널에서 데이터 읽기
    auto* inputChannel = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
    
    // 버퍼에 데이터 저장
    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
        audioBuffer[bufferIndex] = inputChannel[i];
        bufferIndex = (bufferIndex + 1) % bufferSize;
    }
    
    // 출력은 무음 처리
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // 오디오 정지 시 정리
}

void MainComponent::paint(juce::Graphics& g)
{
    // 배경 그리기
    g.fillAll(juce::Colours::black);
    
    auto width = (float)getWidth();
    auto height = (float)getHeight();
    auto centerY = height * 0.5f;
    
    // 파형 그리기
    g.setColour(juce::Colours::lime);
    
    juce::Path waveform;
    waveform.startNewSubPath(0, centerY);
    
    // 버퍼의 데이터를 사용하여 파형 그리기
    for (int x = 0; x < width; ++x)
    {
        // x 좌표에 해당하는 버퍼 인덱스 계산
        int index = (bufferIndex + (int)((float)x * bufferSize / width)) % bufferSize;
        
        // y 좌표 계산 (진폭을 화면 높이의 40%로 스케일링)
        float y = centerY + (audioBuffer[index] * height * 0.4f);
        
        if (x == 0)
            waveform.startNewSubPath(0, y);
        else
            waveform.lineTo(x, y);
    }
    
    // 부드러운 선으로 파형 그리기
    g.strokePath(waveform, juce::PathStrokeType(2.0f));
}

void MainComponent::resized()
{
    // Settings 버튼 위치 설정
    settingsButton.setBounds(10, 10, 100, 30);
}

void MainComponent::timerCallback()
{
    // 화면 갱신
    repaint();
}
