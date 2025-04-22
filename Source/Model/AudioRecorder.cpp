#include "AudioRecorder.h"

AudioRecorder::AudioRecorder(juce::AudioThumbnail& thumbnailToUse)
    : thumbnail(thumbnailToUse), backgroundThread("Audio Recorder Thread")
{
    backgroundThread.startThread();
    formatManager.registerBasicFormats();
}

AudioRecorder::~AudioRecorder()
{
    stop();
    backgroundThread.stopThread(500);
}

void AudioRecorder::startRecording(const juce::File& file)
{
    stop();
    
    if (file.existsAsFile())
        file.deleteFile();
        
    if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream()))
    {
        juce::WavAudioFormat wavFormat;
        
        if (auto writer = wavFormat.createWriterFor(fileStream.get(), 44100.0, 2, 16, {}, 0))
        {
            fileStream.release(); // writer가 파일을 소유
            
            threadedWriter.reset(new juce::AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768));
            
            // 녹음 시작
            nextSampleNum = 0;
            isRecordingFlag = true;
            
            // 썸네일 초기화
            thumbnail.reset(2, 44100.0);
        }
    }
}

void AudioRecorder::stop()
{
    // 쓰기 중지
    threadedWriter.reset();
    isRecordingFlag = false;
}

bool AudioRecorder::isRecording() const noexcept
{
    return isRecordingFlag;
}

void AudioRecorder::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                                        float** outputChannelData, int numOutputChannels,
                                        int numSamples)
{
    // 출력 채널 클리어
    for (int i = 0; i < numOutputChannels; ++i)
        if (outputChannelData[i] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
            
    // 녹음 중이면 데이터 저장
    if (isRecordingFlag && threadedWriter != nullptr && numInputChannels > 0)
    {
        threadedWriter->write((const float**)inputChannelData, numSamples);
        
        // 썸네일 업데이트
        juce::AudioBuffer<float> buffer(const_cast<float**>(inputChannelData), numInputChannels, numSamples);
        thumbnail.addBlock(nextSampleNum, buffer, 0, numSamples);
        nextSampleNum += numSamples;
    }
}

void AudioRecorder::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
}

void AudioRecorder::audioDeviceStopped()
{
    // 필요시 녹음 중지
    if (isRecordingFlag)
        stop();
} 