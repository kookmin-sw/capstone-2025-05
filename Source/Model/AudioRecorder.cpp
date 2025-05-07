#include "AudioRecorder.h"

AudioRecorder::AudioRecorder(juce::AudioThumbnail& thumbnailToUse)
    : thumbnail(thumbnailToUse)
{
    backgroundThread.startThread();
}

AudioRecorder::~AudioRecorder()
{
    stop();
}

void AudioRecorder::startRecording(const juce::File& file)
{
    stop();

    if (sampleRate > 0)
    {
        // 출력 스트림 생성
        file.deleteFile();

        if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream()))
        {
            // WAV 형식의 writer 생성
            juce::WavAudioFormat wavFormat;

            if (auto writer = wavFormat.createWriterFor(fileStream.get(), sampleRate, 1, 16, {}, 0))
            {
                fileStream.release(); // writer가 파일 소유권 획득

                // 백그라운드 스레드에서 데이터를 디스크에 쓰는 ThreadedWriter 생성
                threadedWriter.reset(new juce::AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768));

                // 녹음 미리보기 초기화
                thumbnail.reset(writer->getNumChannels(), writer->getSampleRate());
                nextSampleNum = 0;

                // activeWriter 설정 (오디오 콜백에서 사용)
                const juce::ScopedLock sl(writerLock);
                activeWriter = threadedWriter.get();
            }
        }
    }
}

void AudioRecorder::stop()
{
    // activeWriter 포인터를 클리어하여 오디오 콜백에서 사용하지 않도록 함
    {
        const juce::ScopedLock sl(writerLock);
        activeWriter = nullptr;
    }

    // ThreadedWriter 객체 삭제
    // 남은 데이터가 디스크에 플러시되는 동안 시간이 걸릴 수 있으므로
    // 오디오 콜백을 차단하지 않기 위해 이 순서로 수행
    threadedWriter.reset();
}

bool AudioRecorder::isRecording() const
{
    return activeWriter.load() != nullptr;
}

void AudioRecorder::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
}

void AudioRecorder::audioDeviceStopped()
{
    sampleRate = 0;
}

void AudioRecorder::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                                   float* const* outputChannelData, int numOutputChannels,
                                                   int numSamples, const juce::AudioIODeviceCallbackContext& context)
{
    juce::ignoreUnused(context);
    
    // 디버깅을 위한 로깅 (100번에 1번만 출력)
    static int logCounter = 0;
    bool shouldLog = (++logCounter % 100) == 0;
    
    // 레코딩 상태 확인
    const juce::ScopedLock sl(writerLock);
    bool isActive = activeWriter.load() != nullptr;
    
    if (shouldLog && isActive)
    {
        DBG("AudioRecorder: Recording active, numInputChannels=" + juce::String(numInputChannels));
        
        // 채널별 레벨 로깅
        if (numInputChannels > 0)
        {
            juce::String channelLevels = "Channel levels: ";
            for (int ch = 0; ch < numInputChannels; ++ch)
            {
                if (inputChannelData[ch] != nullptr)
                {
                    float level = 0.0f;
                    for (int i = 0; i < numSamples; ++i)
                        level = juce::jmax(level, std::abs(inputChannelData[ch][i]));
                    
                    channelLevels += "ch" + juce::String(ch) + "=" + juce::String(level, 3) + " ";
                }
                else
                {
                    channelLevels += "ch" + juce::String(ch) + "=null ";
                }
            }
            DBG(channelLevels);
        }
    }

    // 마이크 채널 감지 - 일반적으로 두 번째 채널(인덱스 1)
    int micChannel = -1;
    if (numInputChannels > 1)
    {
        // 마이크 채널 자동 감지 (가장 강한 신호를 가진 채널)
        float highestLevel = 0.01f; // 노이즈 임계값
        
        for (int ch = 0; ch < numInputChannels; ++ch)
        {
            if (inputChannelData[ch] != nullptr)
            {
                float level = 0.0f;
                for (int i = 0; i < numSamples; ++i)
                    level = juce::jmax(level, std::abs(inputChannelData[ch][i]));
                
                if (level > highestLevel)
                {
                    highestLevel = level;
                    micChannel = ch;
                }
            }
        }
        
        // 신호가 너무 약하면 기본값으로 채널 1 사용
        if (micChannel == -1)
            micChannel = 1; // 두 번째 채널이 일반적으로 마이크
    }
    else if (numInputChannels > 0)
    {
        micChannel = 0; // 채널이 하나뿐이면 그 채널 사용
    }
    
    if (shouldLog && isActive)
    {
        DBG("AudioRecorder: Using mic channel " + juce::String(micChannel));
    }

    // 실제 레코딩 처리
    if (isActive && micChannel >= 0 && micChannel < numInputChannels)
    {
        // 마이크 채널 데이터만 사용하여 임시 입력 버퍼 생성
        const float* tempChannelData[2] = { nullptr, nullptr };
        
        // 마이크 채널 데이터를 첫 번째 채널에 복사
        tempChannelData[0] = inputChannelData[micChannel];
        
        // 모노 레코딩 - 마이크 채널만 사용
        activeWriter.load()->write(tempChannelData, numSamples);
        
        // 미리보기 썸네일 업데이트 (마이크 채널만 사용)
        juce::AudioBuffer<float> buffer(const_cast<float**>(&tempChannelData[0]), 1, numSamples);
        thumbnail.addBlock(nextSampleNum, buffer, 0, numSamples);
        nextSampleNum += numSamples;
    }

    // 출력 버퍼 클리어 
    for (int i = 0; i < numOutputChannels; ++i)
        if (outputChannelData[i] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
} 