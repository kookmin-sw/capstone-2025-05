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

void AudioController::setMicrophoneGain(float gain)
{
    // 유효한 범위 내에서 게인 설정 (0.1 ~ 10.0)
    microphoneGain = juce::jlimit(0.1f, 10.0f, gain);
    DBG("AudioController: Microphone gain set to " + juce::String(microphoneGain));
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
    
    // 입력 레벨 계산 및 업데이트
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
        
        // 레벨 업데이트 (리스너에게 알림)
        float newLevel = juce::jmax(audioModel.getCurrentInputLevel() * 0.7f, currentBlockLevel);
        audioModel.setCurrentInputLevel(newLevel);
        
        if (shouldLogMonitoring && microphoneMonitoringEnabled) {
            DBG("AudioController: Input level = " + juce::String(newLevel));
        }
    }
    
    // 출력 버퍼 초기화
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (outputChannelData[channel] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    }
    
    // 효율적인 오디오 처리를 위해 AudioBuffer 참조 사용
    juce::AudioBuffer<float> outputBuffer;
    outputBuffer.setDataToReferTo(const_cast<float**>(outputChannelData), numOutputChannels, numSamples);
    
    // 재생 처리 (GuitarPracticeController)
    if (guitarPracticeController != nullptr)
    {
        bool isPlaying = audioModel.isPlaying();
        
        static int loggingCounter = 0;
        bool shouldLog = (++loggingCounter >= 100);
        if (shouldLog) 
            loggingCounter = 0;
            
        if (shouldLog)
            DBG("AudioController: isPlaying = " + juce::String(isPlaying ? "true" : "false"));
        
        // AudioSourceChannelInfo 객체는 outputBuffer의 참조를 가짐
        juce::AudioSourceChannelInfo bufferInfo(outputBuffer);
        
        // GuitarPracticeController에 오디오 처리 위임 (참조로 전달)
        guitarPracticeController->getNextAudioBlock(bufferInfo);
        
        // 디버깅 정보
        if (shouldLog)
        {
            float maxSample = 0.0f;
            bool hasOutput = false;
            
            for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            {
                auto* data = outputBuffer.getReadPointer(ch);
                for (int i = 0; i < numSamples; ++i)
                {
                    maxSample = juce::jmax(maxSample, std::abs(data[i]));
                    if (std::abs(data[i]) > 0.0001f)
                        hasOutput = true;
                }
            }
            
            if (!hasOutput)
                DBG("AudioController: No audio data in output buffer (silence)");
            else
                DBG("AudioController: Audio data present in buffer, max sample = " + juce::String(maxSample));
        }
    }
    
    // 마이크 모니터링 처리 (참조를 통해 직접 출력 버퍼 조작)
    if (microphoneMonitoringEnabled && numInputChannels > 0 && numOutputChannels > 0)
    {
        if (shouldLogMonitoring) {
            DBG("AudioController: Processing microphone monitoring using references");
        }
        
        float volume = audioModel.getVolume();
        float monitorGain = microphoneGain * volume;
        
        // 스테레오 출력 (2채널 이상)
        if (numOutputChannels >= 2)
        {
            // 입력 채널 데이터에 직접 접근 (복사 없음)
            int inputChannel = 0; // 첫 번째 채널 사용
            
            if (inputChannelData[inputChannel] != nullptr)
            {
                // 왼쪽 채널 처리 (출력 버퍼 0번)
                float leftGain = monitorGain;
                auto* leftOut = outputBuffer.getWritePointer(0);
                
                // 오른쪽 채널 처리 (출력 버퍼 1번)
                float rightGain = monitorGain * 0.98f;
                auto* rightOut = outputBuffer.getWritePointer(1);
                
                // 하나의 루프에서 처리 (효율성 증가)
                for (int i = 0; i < numSamples; ++i)
                {
                    // 왼쪽 채널 - 원본 신호
                    float leftSample = inputChannelData[inputChannel][i] * leftGain;
                    leftOut[i] += juce::jlimit(-1.0f, 1.0f, leftSample);
                    
                    // 오른쪽 채널 - 지연된 신호 (스테레오 효과)
                    float delayedSample = (i > 0) ? inputChannelData[inputChannel][i-1] : 0.0f;
                    float rightSample = delayedSample * rightGain;
                    rightOut[i] += juce::jlimit(-1.0f, 1.0f, rightSample);
                }
                
                // 스테레오 입력이 있으면 두 번째 채널도 활용
                if (numInputChannels >= 2 && inputChannelData[1] != nullptr)
                {
                    float secondInputGain = monitorGain * 0.5f;
                    // 두 번째 입력 채널을 오른쪽 출력에 추가 (참조 사용)
                    for (int i = 0; i < numSamples; ++i)
                    {
                        float inputSample = inputChannelData[1][i] * secondInputGain;
                        rightOut[i] += juce::jlimit(-1.0f, 1.0f, inputSample);
                    }
                }
            }
        }
        else // 모노 출력 (1채널)
        {
            // 단일 채널 출력 - 가장 효율적인 처리
            for (int outChannel = 0; outChannel < numOutputChannels; ++outChannel)
            {
                if (outputChannelData[outChannel] != nullptr)
                {
                    int inChannel = outChannel % numInputChannels;
                    
                    if (inputChannelData[inChannel] != nullptr)
                    {
                        auto* outPtr = outputBuffer.getWritePointer(outChannel);
                        const auto* inPtr = inputChannelData[inChannel];
                        
                        // SIMD 최적화 가능한 벡터 연산 사용
                        for (int i = 0; i < numSamples; ++i)
                        {
                            outPtr[i] += juce::jlimit(-1.0f, 1.0f, inPtr[i] * monitorGain);
                        }
                    }
                }
            }
        }
        
        // 모니터링 출력 레벨 로깅
        if (shouldLogMonitoring)
        {
            float outputLevel = outputBuffer.getMagnitude(0, numSamples);
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