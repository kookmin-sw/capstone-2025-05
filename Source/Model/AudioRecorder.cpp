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
    
    const juce::ScopedLock sl(writerLock);

    if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels())
    {
        activeWriter.load()->write(inputChannelData, numSamples);

        // 오디오 데이터를 참조하는 AudioBuffer 생성 (메모리 할당 또는 복사 없음)
        juce::AudioBuffer<float> buffer(const_cast<float**>(inputChannelData), thumbnail.getNumChannels(), numSamples);
        thumbnail.addBlock(nextSampleNum, buffer, 0, numSamples);
        nextSampleNum += numSamples;
    }

    // 출력 버퍼 클리어
    for (int i = 0; i < numOutputChannels; ++i)
        if (outputChannelData[i] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
} 