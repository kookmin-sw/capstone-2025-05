#include "PracticeSongComponent.h"

PracticeSongComponent::PracticeSongComponent()
{
    setAudioChannels(2, 2);
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(leftPanel);
    addAndMakeVisible(rightPanel);
    addAndMakeVisible(centralContent);
    addAndMakeVisible(waveformDisplay);
}

PracticeSongComponent::~PracticeSongComponent()
{
    shutdownAudio();
}

void PracticeSongComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // 오디오 처리 준비
}

void PracticeSongComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // 실시간 오디오 처리 (레벨 미터, 녹음 등)
}

void PracticeSongComponent::releaseResources()
{
    // 오디오 리소스 해제
}

void PracticeSongComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    // 중앙 콘텐츠 영역 (임시)
    g.setColour(juce::Colours::grey);
    g.fillRect(centralContent.getBounds());
    g.setColour(juce::Colours::white);
    g.drawText(juce::String::fromUTF8("Sheet Music / Album Cover"), 
               centralContent.getBounds(),
               juce::Justification::centred);
               
    // 파형 표시 영역 (임시)
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(waveformDisplay.getBounds());
    g.setColour(juce::Colours::white);
    g.drawText(juce::String::fromUTF8("Waveform Display"), 
               waveformDisplay.getBounds(),
               juce::Justification::centred);
}

void PracticeSongComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // 헤더 영역 (상단 10%)
    headerPanel.setBounds(bounds.removeFromTop(proportionOfHeight(0.1f)));
    
    // 하단 파형 표시 영역 (하단 20%)
    waveformDisplay.setBounds(bounds.removeFromBottom(proportionOfHeight(0.25f)));
    
    // 좌우 패널 (각각 15% 너비)
    leftPanel.setBounds(bounds.removeFromLeft(proportionOfWidth(0.15f)));
    rightPanel.setBounds(bounds.removeFromRight(proportionOfWidth(0.15f)));
    
    // 중앙 콘텐츠 영역 (나머지 공간)
    centralContent.setBounds(bounds);
} 