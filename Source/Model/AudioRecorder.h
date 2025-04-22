#pragma once
#include <JuceHeader.h>

/**
 * 오디오 녹음을 담당하는 클래스
 * AudioIODeviceCallback을 구현하여 오디오 입력을 파일에 저장합니다.
 */
class AudioRecorder : public juce::AudioIODeviceCallback
{
public:
    /**
     * AudioRecorder 생성자
     * @param thumbnailToUse 녹음 상태를 시각적으로 표시할 AudioThumbnail
     */
    AudioRecorder(juce::AudioThumbnail& thumbnailToUse);
    ~AudioRecorder() override;
    
    /**
     * 녹음 시작
     * @param file 녹음을 저장할 파일
     */
    void startRecording(const juce::File& file);
    
    /**
     * 녹음 중지
     */
    void stop();
    
    /**
     * 현재 녹음 중인지 상태 확인
     * @return 녹음 중이면 true, 아니면 false
     */
    bool isRecording() const noexcept;
    
    // AudioIODeviceCallback 인터페이스 구현
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                             float** outputChannelData, int numOutputChannels,
                             int numSamples);
    void audioDeviceAboutToStart(juce::AudioIODevice* device);
    void audioDeviceStopped();
    
private:
    juce::AudioFormatManager formatManager;
    juce::TimeSliceThread backgroundThread{"Audio Recorder Thread"};
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    juce::AudioThumbnail& thumbnail;
    
    double sampleRate = 0.0;
    juce::int64 nextSampleNum = 0;
    bool isRecordingFlag = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioRecorder)
}; 