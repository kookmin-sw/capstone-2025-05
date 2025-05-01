#include "AudioController.h"
#include "GuitarPracticeController.h"

AudioController::AudioController(AudioModel& model, juce::AudioDeviceManager& manager)
    : audioModel(model), deviceManager(manager), guitarPracticeController(nullptr)
{
    deviceManager.addAudioCallback(this);
}

AudioController::~AudioController()
{
    deviceManager.removeAudioCallback(this);
}

void AudioController::setGuitarPracticeController(GuitarPracticeController* controller)
{
    guitarPracticeController = controller;
}

void AudioController::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                      int numInputChannels,
                                                      float* const* outputChannelData,
                                                      int numOutputChannels,
                                                      int numSamples,
                                                      const juce::AudioIODeviceCallbackContext& context)
{
    // Calculate input level
    if (numInputChannels > 0 && inputChannelData != nullptr)
    {
        float currentBlockLevel = 0.0f;
        
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
        
        // Update model with smoothing (which will notify listeners)
        float newLevel = juce::jmax(audioModel.getCurrentInputLevel() * 0.7f, currentBlockLevel);
        audioModel.setCurrentInputLevel(newLevel);
    }
    
    // Clear output buffers first
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (outputChannelData[channel] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    }
    
    // 오디오 파일 재생이 설정되었고 재생 중이면 오디오 블록 처리
    if (guitarPracticeController != nullptr)
    {
        bool isPlaying = audioModel.isPlaying();
        
        // 1초마다 로깅할 플래그
        static int loggingCounter = 0;
        bool shouldLog = (++loggingCounter >= 100);
        if (shouldLog) 
            loggingCounter = 0;
            
        if (shouldLog)
            DBG("AudioController: isPlaying = " + juce::String(isPlaying ? "true" : "false"));
        
        // 재생 상태와 상관없이 항상 guitarPracticeController에 오디오 처리를 위임
        // (자체적으로 transportSource.isPlaying()을 확인함)
        
        // JUCE에서 권장하는 방식으로 임시 버퍼 생성
        juce::AudioBuffer<float> tempBuffer;
        tempBuffer.setDataToReferTo(const_cast<float**>(outputChannelData), numOutputChannels, numSamples);
        
        // 올바른 방식으로 AudioSourceChannelInfo 생성 (참조로 전달)
        juce::AudioSourceChannelInfo bufferInfo(tempBuffer);
        
        if (shouldLog)
            DBG("AudioController: Calling getNextAudioBlock with numChannels=" + 
                juce::String(tempBuffer.getNumChannels()) + 
                ", numSamples=" + juce::String(numSamples));
        
        // GuitarPracticeController를 통해 오디오 데이터 채우기
        guitarPracticeController->getNextAudioBlock(bufferInfo);
        
        // 출력 버퍼에 데이터가 채워졌는지 확인 (디버깅)
        bool hasOutput = false;
        float maxSample = 0.0f;
        
        for (int ch = 0; ch < tempBuffer.getNumChannels(); ++ch)
        {
            auto* data = tempBuffer.getReadPointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                maxSample = juce::jmax(maxSample, std::abs(data[i]));
                if (std::abs(data[i]) > 0.0001f)
                    hasOutput = true;
            }
        }
        
        // 로그 출력
        if (shouldLog)
        {
            if (!hasOutput)
                DBG("AudioController: No audio data in output buffer (silence)");
            else
                DBG("AudioController: Audio data present in buffer, max sample level = " + juce::String(maxSample));
        }
    }
}

void AudioController::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    audioModel.setCurrentInputLevel(0.0f);
    
    // 오디오 디바이스 시작 시 GuitarPracticeController 준비
    if (guitarPracticeController != nullptr)
    {
        double sampleRate = device->getCurrentSampleRate();
        int samplesPerBlock = device->getCurrentBufferSizeSamples();
        
        DBG("AudioController: Preparing to play with sampleRate = " + juce::String(sampleRate) + 
            ", samplesPerBlock = " + juce::String(samplesPerBlock));
            
        guitarPracticeController->prepareToPlay(samplesPerBlock, sampleRate);
    }
}

void AudioController::audioDeviceStopped()
{
    audioModel.setCurrentInputLevel(0.0f);
    
    // 오디오 디바이스 중지 시 GuitarPracticeController 리소스 해제
    if (guitarPracticeController != nullptr)
    {
        DBG("AudioController: Releasing resources");
        guitarPracticeController->releaseResources();
    }
}