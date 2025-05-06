#pragma once
#include <JuceHeader.h>

class AmpliTubeProcessor
{
public:
    AmpliTubeProcessor();
    ~AmpliTubeProcessor();
    
    // 플러그인 초기화
    bool init();
    
    // 오디오 프로세싱
    void processBlock(const float* const* inputChannelData, 
                     int numInputChannels,
                     float* const* outputChannelData, 
                     int numOutputChannels,
                     int numSamples);
    
    // 플러그인 에디터 컴포넌트 가져오기
    juce::Component* getEditorComponent();
    
    // 프로세싱 활성화/비활성화
    void setProcessingEnabled(bool shouldBeEnabled);
    bool isProcessingEnabled() const { return processingEnabled; }
    
    // 샘플링 레이트 설정
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    
private:
    std::unique_ptr<juce::AudioPluginInstance> plugin;
    std::unique_ptr<juce::AudioPluginFormatManager> formatManager;
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    
    juce::AudioBuffer<float> tempBuffer;
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    bool processingEnabled = false;
    
    // 플러그인 찾기 및 로드 메서드
    juce::AudioPluginInstance* findAndLoadPlugin();
}; 