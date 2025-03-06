#include "PracticeSongComponent.h"
#include "MainComponent.h"
#include "TopBar.h"
#include "CenterPanel.h"
#include "LeftPanel.h"
#include "RightPanel.h"
#include "EffectControls.h"
#include "AudioPlaying.h"

PracticeSongComponent::PracticeSongComponent(MainComponent& mainComp)
    : mainComponent(mainComp), deviceManager(mainComp.getDeviceManager())
{
    // MainComponent에서 오디오 장치 관리자를 가져와 사용
    // deviceManager = mainComponent.getDeviceManager();
    
    // 오디오 장치 관리자에 콜백 등록
    deviceManager.addAudioCallback(this);
    
    topBar = std::make_unique<TopBar>(*this);
    centerPanel = std::make_unique<CenterPanel>();
    leftPanel = std::make_unique<LeftPanel>();
    rightPanel = std::make_unique<RightPanel>();
    effectControls = std::make_unique<EffectControls>();
    // waveformGraph = std::make_unique<WaveformGraph>();
    waveformGraph = std::make_unique<AudioPlaybackDemo>();

    addAndMakeVisible(topBar.get());
    addAndMakeVisible(centerPanel.get());
    addAndMakeVisible(leftPanel.get());
    addAndMakeVisible(rightPanel.get());
    addAndMakeVisible(effectControls.get());
    addAndMakeVisible(waveformGraph.get());
}

PracticeSongComponent::~PracticeSongComponent()
{
    deviceManager.removeAudioCallback(this);
}

void PracticeSongComponent::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                                 float* const* outputChannelData, int numOutputChannels, int numSamples,
                                                 const juce::AudioIODeviceCallbackContext& context)
{
    // 입력 레벨 계산
    if (numInputChannels > 0 && inputChannelData != nullptr)
    {
        float currentBlockLevel = 0.0f;
        
        // 모든 입력 채널의 최대 레벨 계산
        for (int channel = 0; channel < numInputChannels; ++channel)
        {
            if (inputChannelData[channel] != nullptr)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    float sample = std::abs(inputChannelData[channel][i]);
                    currentBlockLevel = juce::jmax(currentBlockLevel, sample);
                }
            }
        }
        
        // 현재 레벨 업데이트 (약간의 스무딩 적용)
        currentInputLevel = juce::jmax(currentInputLevel * 0.7f, currentBlockLevel);
        
        // 레벨 미터 업데이트
        // leftPanel.getLevelMeter().setLevel(currentInputLevel);
    }
    
    // 실시간 오디오 처리 (레벨 미터, 녹음 등)
    // 기본적으로 출력 버퍼를 비웁니다 (무음 출력)
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (outputChannelData[channel] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    }
}

void PracticeSongComponent::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    // 오디오 처리 준비
    currentInputLevel = 0.0f;
}

void PracticeSongComponent::audioDeviceStopped()
{
    // 오디오 리소스 해제
    currentInputLevel = 0.0f;
}

void PracticeSongComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PracticeSongComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    transport.getNextAudioBlock(bufferToFill);
}

void PracticeSongComponent::releaseResources() {
    transport.releaseResources();
}

void PracticeSongComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    // // 중앙 콘텐츠 영역 (임시)
    // g.setColour(juce::Colours::grey);
    // g.fillRect(centralContent.getBounds());
    // g.setColour(juce::Colours::white);
    // g.drawText(juce::String::fromUTF8("Sheet Music / Album Cover"), 
    //            centralContent.getBounds(),
    //            juce::Justification::centred);
               
    // // 파형 표시 영역 (임시)
    // g.setColour(juce::Colours::darkgrey);
    // g.fillRect(waveformDisplay.getBounds());
    // g.setColour(juce::Colours::white);
    // g.drawText(juce::String::fromUTF8("Waveform Display"), 
    //            waveformDisplay.getBounds(),
    //            juce::Justification::centred);
}

void PracticeSongComponent::resized()
{
    auto bounds = getLocalBounds();
    topBar->setBounds(bounds.removeFromTop(50));
    waveformGraph->setBounds(bounds.removeFromBottom(300));
    effectControls->setBounds(bounds.removeFromBottom(100));
    leftPanel->setBounds(bounds.removeFromLeft(300));
    rightPanel->setBounds(bounds.removeFromRight(300));
    centerPanel->setBounds(bounds.reduced(50));
    // auto bounds = getLocalBounds();
    
    // // 헤더 영역 (상단 10%)
    // headerPanel.setBounds(bounds.removeFromTop(proportionOfHeight(0.1f)));
    
    // // 하단 파형 표시 영역 (하단 20%)
    // waveformDisplay.setBounds(bounds.removeFromBottom(proportionOfHeight(0.25f)));
    
    // // 좌우 패널 (각각 15% 너비)
    // leftPanel.setBounds(bounds.removeFromLeft(proportionOfWidth(0.15f)));
    // rightPanel.setBounds(bounds.removeFromRight(proportionOfWidth(0.15f)));
    
    // // 중앙 콘텐츠 영역 (나머지 공간)
    // centralContent.setBounds(bounds);
} 