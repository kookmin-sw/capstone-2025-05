#include "AudioController.h"
#include "GuitarPracticeController.h"

AudioController::AudioController(AudioModel& model, juce::AudioDeviceManager& manager)
    : audioModel(model), deviceManager(manager), guitarPracticeController(nullptr),
      microphoneMonitoringEnabled(false), microphoneGain(5.0f)
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

void AudioController::enableMicrophoneMonitoring(bool shouldEnable)
{
    microphoneMonitoringEnabled = shouldEnable;
    DBG("Monitoring " + juce::String(shouldEnable ? "enabled" : "disabled"));
    
    if (shouldEnable) {
        DBG("AudioController: Microphone monitoring is now ENABLED. Gain = " + juce::String(microphoneGain));
    } else {
        DBG("AudioController: Microphone monitoring is now DISABLED");
    }
}

void AudioController::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                      int numInputChannels,
                                                      float* const* outputChannelData,
                                                      int numOutputChannels,
                                                      int numSamples,
                                                      const juce::AudioIODeviceCallbackContext& context)
{
    static int monitoringLogCounter = 0;
    bool shouldLogMonitoring = (++monitoringLogCounter >= 100);
    if (shouldLogMonitoring) 
        monitoringLogCounter = 0;
    
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
        
        if (shouldLogMonitoring && microphoneMonitoringEnabled) {
            DBG("AudioController: Input level = " + juce::String(newLevel) + 
                ", Monitoring enabled = " + juce::String(microphoneMonitoringEnabled ? "yes" : "no"));
        }
    }
    
    // 버퍼 처리 순서 변경: 재생 먼저 처리 후 마이크 모니터링
    
    // 1. 출력 버퍼 초기화
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (outputChannelData[channel] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    }
    
    // 2. 먼저 오디오 파일 재생 처리 (출력 버퍼에 채우기)
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
    
    // 3. 그 다음 마이크 모니터링 처리 (출력 버퍼에 추가)
    if (microphoneMonitoringEnabled && numInputChannels > 0 && numOutputChannels > 0)
    {
        if (shouldLogMonitoring) {
            DBG("AudioController: Processing microphone monitoring: inChannels=" + 
                juce::String(numInputChannels) + ", outChannels=" + juce::String(numOutputChannels));
        }
        
        // 현재 볼륨 설정 가져오기
        float volume = audioModel.getVolume();
        // 모니터링을 위한 실제 게인 계산 (볼륨값 적용)
        float monitorGain = microphoneGain * volume;
        
        if (shouldLogMonitoring) {
            DBG("AudioController: Monitoring gain = " + juce::String(monitorGain) + 
                " (base=" + juce::String(microphoneGain) + 
                ", volume=" + juce::String(volume) + ")");
        }
        
        // 모든 출력 채널에 입력 신호 복사 (모노->스테레오 또는 멀티채널)
        for (int outChannel = 0; outChannel < numOutputChannels; ++outChannel)
        {
            if (outputChannelData[outChannel] != nullptr)
            {
                // 해당 출력 채널에 대응하는 입력 채널 결정 (순환적으로)
                int inChannel = outChannel % numInputChannels;
                
                if (inputChannelData[inChannel] != nullptr)
                {
                    // 입력 데이터를 게인 값을 적용하여 출력으로 복사 (기존 출력에 더함)
                    for (int i = 0; i < numSamples; ++i)
                    {
                        // 클리핑 방지를 위한 최대값 제한
                        float inputSample = inputChannelData[inChannel][i] * monitorGain;
                        // 기존 출력에 모니터링 신호 더하기 (필요시 클리핑 방지)
                        outputChannelData[outChannel][i] += juce::jlimit(-1.0f, 1.0f, inputSample);
                    }
                }
            }
        }
        
        if (shouldLogMonitoring) {
            float outputLevel = 0.0f;
            for (int ch = 0; ch < numOutputChannels; ++ch) {
                if (outputChannelData[ch] != nullptr) {
                    for (int i = 0; i < numSamples; ++i) {
                        outputLevel = juce::jmax(outputLevel, std::abs(outputChannelData[ch][i]));
                    }
                }
            }
            DBG("AudioController: Final output level (after monitoring) = " + juce::String(outputLevel));
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