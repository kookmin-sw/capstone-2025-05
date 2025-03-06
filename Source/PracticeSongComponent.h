#pragma once
#include <JuceHeader.h>
#include "WaveformGraph.h"

class MainComponent;  // 전방 선언
class TopBar;
class CenterPanel;
class LeftPanel;
class RightPanel;
class EffectControls;
class AudioPlaybackDemo;

class PracticeSongComponent : public juce::Component,
                              public juce::AudioIODeviceCallback,
                              public juce::AudioSource
{
public:
    PracticeSongComponent(MainComponent& mainComp);
    ~PracticeSongComponent() override;

    // AudioIODeviceCallback 인터페이스 구현
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                              float* const* outputChannelData, int numOutputChannels, int numSamples,
                              const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // TopBar가 사용할 public 인터페이스
    void loadAudioFile(const juce::File& file);
    // WaveformGraph* getWaveformGraph() { return waveformGraph.get(); }
    juce::AudioTransportSource& getTransport() { return transport; }
    juce::AudioFormatManager& getFormatManager() { return formatManager; }

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // 레벨 미터 클래스
    class LevelMeterComponent : public juce::Component, private juce::Timer
    {
    public:
        LevelMeterComponent()
        {
            startTimerHz(24); // 초당 24번 업데이트
        }

        ~LevelMeterComponent() override
        {
            stopTimer();
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();
            
            // 배경
            g.setColour(juce::Colours::black);
            g.fillRect(bounds);
            
            // 레벨 표시
            const float levelHeight = bounds.getHeight() * level;
            juce::Rectangle<float> levelRect(bounds.getX(), bounds.getBottom() - levelHeight, 
                                           bounds.getWidth(), levelHeight);
            
            // 레벨에 따라 색상 변경
            if (level > 0.8f)
                g.setColour(juce::Colours::red);
            else if (level > 0.5f)
                g.setColour(juce::Colours::orange);
            else
                g.setColour(juce::Colours::green);
                
            g.fillRect(levelRect);
            
            // 눈금 표시
            g.setColour(juce::Colours::white);
            for (int i = 1; i < 10; ++i)
            {
                float y = bounds.getY() + bounds.getHeight() * i / 10.0f;
                g.drawLine(bounds.getX(), y, bounds.getRight(), y, 0.5f);
            }
        }

        void setLevel(float newLevel)
        {
            // 0.0 ~ 1.0 범위로 제한
            level = juce::jlimit(0.0f, 1.0f, newLevel);
        }

    private:
        float level = 0.0f;
        
        void timerCallback() override
        {
            // 레벨이 서서히 감소하도록 함
            level = level * 0.9f;
            repaint();
        }
    };

    std::unique_ptr<TopBar> topBar;
    std::unique_ptr<CenterPanel> centerPanel;
    std::unique_ptr<LeftPanel> leftPanel;
    std::unique_ptr<RightPanel> rightPanel;
    std::unique_ptr<EffectControls> effectControls;
    // std::unique_ptr<WaveformGraph> waveformGraph;
    std::unique_ptr<AudioPlaybackDemo> waveformGraph;


    juce::AudioDeviceManager& deviceManager;
    juce::AudioSourcePlayer audioPlayer;
    juce::AudioTransportSource transport;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    MainComponent& mainComponent;  // MainComponent에 대한 참조
    
    // 오디오 레벨 계산을 위한 변수
    float currentInputLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PracticeSongComponent)
}; 