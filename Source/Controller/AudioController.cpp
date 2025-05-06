#include "AudioController.h"
#include "GuitarPracticeController.h"

AudioController::AudioController(AudioModel& model, juce::AudioDeviceManager& manager)
    : audioModel(model), deviceManager(manager), guitarPracticeController(nullptr),
      microphoneMonitoringEnabled(false), microphoneGain(5.0f)
{
    // AmpliTube 프로세서 초기화 - 기본적으로 비활성화 상태로 시작
    ampliTubeProcessor = std::make_unique<AmpliTubeProcessor>();
    // 명시적 비활성화 설정 (초기 상태 보장)
    if (ampliTubeProcessor)
        ampliTubeProcessor->setProcessingEnabled(false);
    
    DBG("AudioController: Created with AmpliTube initially DISABLED");
    deviceManager.addAudioCallback(this);
}

AudioController::~AudioController()
{
    deviceManager.removeAudioCallback(this);
    ampliTubeProcessor = nullptr;
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

// AmpliTube 관련 메서드 구현
void AudioController::enableAmpliTubeEffect(bool shouldEnable)
{
    if (ampliTubeProcessor) {
        // 상태가 변경될 때만 로그 출력
        if (ampliTubeProcessor->isProcessingEnabled() != shouldEnable) {
            DBG("AudioController: AmpliTube effect state changing from " + 
                juce::String(ampliTubeProcessor->isProcessingEnabled() ? "enabled" : "disabled") + 
                " to " + juce::String(shouldEnable ? "enabled" : "disabled"));
        }
        
        // 처음 활성화될 때 초기화
        if (shouldEnable && !ampliTubeProcessor->isProcessingEnabled()) {
            // 아직 초기화되지 않았다면 초기화
            if (!ampliTubeProcessor->getEditorComponent()) {
                DBG("AudioController: Initializing AmpliTube plugin");
                bool initialized = ampliTubeProcessor->init();
                if (!initialized) {
                    DBG("AudioController: Failed to initialize AmpliTube");
                    return;
                }
                DBG("AudioController: AmpliTube plugin initialized successfully");
            }
        }
        
        // 활성화/비활성화 상태 설정
        ampliTubeProcessor->setProcessingEnabled(shouldEnable);
        DBG("AudioController: AmpliTube effect " + juce::String(shouldEnable ? "enabled" : "disabled"));
    }
}

bool AudioController::isAmpliTubeEffectEnabled() const
{
    if (ampliTubeProcessor)
        return ampliTubeProcessor->isProcessingEnabled();
    return false;
}

juce::Component* AudioController::getAmpliTubeEditorComponent()
{
    if (ampliTubeProcessor) {
        // 아직 초기화되지 않았다면 초기화
        if (!ampliTubeProcessor->getEditorComponent()) {
            bool initialized = ampliTubeProcessor->init();
            if (!initialized) {
                DBG("AudioController: Failed to initialize AmpliTube editor");
                return nullptr;
            }
        }
        return ampliTubeProcessor->getEditorComponent();
    }
    return nullptr;
}

void AudioController::setAmpliTubeInputGain(float gain)
{
    if (ampliTubeProcessor) {
        ampliTubeProcessor->setInputGain(gain);
        DBG("AudioController: AmpliTube input gain set to " + juce::String(gain));
    }
}

float AudioController::getAmpliTubeInputGain() const
{
    if (ampliTubeProcessor)
        return ampliTubeProcessor->getInputGain();
    return 0.5f; // 기본값
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
        
        // 모니터링이 활성화된 경우 이 지점에서 버퍼 상태를 기록
        if (microphoneMonitoringEnabled && shouldLog) {
            DBG("AudioController: Before GPC - Monitoring enabled, checking if buffer has content");
            float bufferLevelBefore = outputBuffer.getMagnitude(0, numSamples);
            DBG("AudioController: Before GPC - Buffer level: " + juce::String(bufferLevelBefore));
        }
        
        // GuitarPracticeController에 오디오 처리 위임 (참조로 전달)
        guitarPracticeController->getNextAudioBlock(bufferInfo);
        
        // 모니터링이 활성화된 경우 GPC 처리 후 버퍼 상태 기록
        if (microphoneMonitoringEnabled && shouldLog) {
            DBG("AudioController: After GPC - Checking if GPC added audio to buffer");
            float bufferLevelAfter = outputBuffer.getMagnitude(0, numSamples);
            DBG("AudioController: After GPC - Buffer level: " + juce::String(bufferLevelAfter));
        }
        
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
            DBG("AudioController: Output buffer before any monitoring code: " + juce::String(outputBuffer.getMagnitude(0, numSamples)));
        }
        
        float volume = audioModel.getVolume();
        float monitorGain = microphoneGain * volume;
        
        // AmpliTube 이펙트 처리 (입력 버퍼를 처리하여 출력 버퍼에 추가)
        if (ampliTubeProcessor && ampliTubeProcessor->isProcessingEnabled())
        {
            if (shouldLogMonitoring) {
                DBG("AudioController: AmpliTube is ENABLED, processing audio with effects");
            }
            
            // 동적 메모리 할당은 힙 메모리를 사용합니다
            std::unique_ptr<float[]> tempBuffer1 = std::make_unique<float[]>(numSamples);
            std::unique_ptr<float[]> tempBuffer2 = std::make_unique<float[]>(numSamples);
            
            float* tempOutputData[2] = { nullptr, nullptr };
            
            if (numOutputChannels >= 1)
                tempOutputData[0] = tempBuffer1.get();
            if (numOutputChannels >= 2)
                tempOutputData[1] = tempBuffer2.get();
            
            // 버퍼 초기화
            for (int ch = 0; ch < juce::jmin(2, numOutputChannels); ++ch)
                if (tempOutputData[ch] != nullptr)
                    std::memset(tempOutputData[ch], 0, sizeof(float) * numSamples);
            
            // 입력 채널 설정 - 채널 1번 (두번째 채널) a사용
            const float* inputChannelToProcess[2] = { nullptr, nullptr };
            if (numInputChannels > 1)
                inputChannelToProcess[0] = inputChannelData[1]; // 마이크 입력 (두번째 채널)
            else if (numInputChannels > 0)
                inputChannelToProcess[0] = inputChannelData[0]; // 대체 채널
                
            // 필요시 두번째 채널 설정 (모노 입력을 스테레오로 복제)
            inputChannelToProcess[1] = inputChannelToProcess[0];
            
            if (shouldLogMonitoring && inputChannelToProcess[0] != nullptr) {
                DBG("AudioController: Input sample to AmpliTube[0]: " + juce::String(inputChannelToProcess[0][0]));
            }
            
            // AmpliTube 프로세서로 오디오 처리
            ampliTubeProcessor->processBlock(inputChannelToProcess, 
                                            inputChannelToProcess[0] != nullptr ? 2 : 0, 
                                            tempOutputData, 
                                            juce::jmin(2, numOutputChannels),
                                            numSamples);
            
            // 처리된 결과를 출력 버퍼에 추가 (볼륨 적용)
            for (int ch = 0; ch < juce::jmin(2, numOutputChannels); ++ch)
            {
                if (tempOutputData[ch] != nullptr && outputChannelData[ch] != nullptr)
                {
                    auto* outPtr = outputBuffer.getWritePointer(ch);
                    const auto* processedPtr = tempOutputData[ch];
                    
                    for (int i = 0; i < numSamples; ++i)
                    {
                        // 볼륨은 이미 AmpliTube에서 처리되므로 추가 게인 없이 적용
                        // 단, 출력 레벨 제어를 위해 기본 볼륨만 적용 (마이크 게인은 제외)
                        outPtr[i] += juce::jlimit(-1.0f, 1.0f, processedPtr[i] * audioModel.getVolume());
                    }
                }
            }
            
            if (shouldLogMonitoring) {
                DBG("AudioController: Applied AmpliTube processing");
                if (tempOutputData[0] != nullptr) {
                    DBG("AudioController: First sample after AmpliTube processing: " + juce::String(tempOutputData[0][0]));
                }
            }
        }
        else // AmpliTube 처리 없이 직접 모니터링
        {
            if (shouldLogMonitoring) {
                DBG("AudioController: AmpliTube is DISABLED, doing direct monitoring");
            }
            
            // 입력 채널 데이터에 직접 접근 (복사 없음)
            int inputChannel = 1; // 마이크 입력 (두번째 채널)
            
            // 유효성 검사: 채널이 범위를 벗어날 경우 첫 번째 사용 가능한 채널로 대체
            if (inputChannel >= numInputChannels && numInputChannels > 0) {
                inputChannel = 0;
                if (shouldLogMonitoring) {
                    DBG("AudioController: Adjusting input channel from 1 to 0 due to channel count limitation");
                }
            }
            
            if (inputChannelData[inputChannel] != nullptr && inputChannel < numInputChannels)
            {
                // 왼쪽 채널 처리 (출력 버퍼 0번)
                float leftGain = monitorGain;
                auto* leftOut = outputBuffer.getWritePointer(0);
                
                // 오른쪽 채널 처리 (출력 버퍼 1번)
                float rightGain = monitorGain * 0.98f;
                auto* rightOut = outputBuffer.getWritePointer(1);
                
                if (shouldLogMonitoring) {
                    // 처리 전 샘플 기록
                    if (numSamples > 0) {
                        float firstInputSample = std::abs(inputChannelData[inputChannel][0]);
                        DBG("AudioController: First sample from input[1]: " + juce::String(firstInputSample));
                        DBG("AudioController: Applied gain (left): " + juce::String(leftGain));
                        DBG("AudioController: Current leftOut[0] before adding: " + juce::String(leftOut[0]));
                    }
                }
                
                // 하나의 루프에서 처리 (효율성 증가)
                for (int i = 0; i < numSamples; ++i)
                {
                    // 왼쪽 채널 - 원본 신호
                    float leftSample = inputChannelData[inputChannel][i] * leftGain;
                    leftOut[i] += juce::jlimit(-1.0f, 1.0f, leftSample);
                    
                    // 오른쪽 채널
                    float rightSample = inputChannelData[inputChannel][i] * rightGain;
                    rightOut[i] += juce::jlimit(-1.0f, 1.0f, rightSample);
                }
                
                if (shouldLogMonitoring) {
                    // 처리 후 샘플 기록
                    if (numSamples > 0) {
                        DBG("AudioController: After processing, leftOut[0]: " + juce::String(leftOut[0]));
                        DBG("AudioController: Buffer level after left channel: " + juce::String(outputBuffer.getMagnitude(0, numSamples)));
                    }
                }
            }
        }
        
        if (shouldLogMonitoring) {
            DBG("AudioController: Final buffer level after all processing: " + juce::String(outputBuffer.getMagnitude(0, numSamples)));
        }
    }
}

void AudioController::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    double sampleRate = device->getCurrentSampleRate();
    int bufferSize = device->getCurrentBufferSizeSamples();
    
    DBG("AudioController: Device about to start - SampleRate=" + juce::String(sampleRate) + 
        ", BufferSize=" + juce::String(bufferSize));
    
    // AmpliTube 프로세서 준비
    if (ampliTubeProcessor)
    {
        ampliTubeProcessor->prepareToPlay(sampleRate, bufferSize);
    }
}

void AudioController::audioDeviceStopped()
{
    DBG("AudioController: Device stopped");
}