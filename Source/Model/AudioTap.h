#pragma once
#include <JuceHeader.h>

/**
 * AudioTap - 실시간 오디오 처리에 안전한 락프리 오디오 데이터 탭
 * 오디오 스레드에서 GUI 스레드로 안전하게 데이터를 전달하는 FIFO 기반 구현
 * 여러 오디오 소스(마이크, 재생)를 구분하여 처리
 */
class AudioTap
{
public:
    // 오디오 소스 타입 정의
    enum class SourceType {
        Microphone,
        Playback,
        Mixed
    };

    /**
     * 생성자 - 버퍼 크기 및 채널 수 초기화
     * @param numChannels 지원할 채널 수 (기본 2)
     * @param bufferSize FIFO 버퍼 크기 (기본 16384)
     */
    AudioTap(int numChannels = 2, int bufferSize = 16384)
        : micFifo(bufferSize), micBuffer(numChannels, bufferSize),
          playbackFifo(bufferSize), playbackBuffer(numChannels, bufferSize),
          currentSourceType(SourceType::Microphone)
    {
        // 읽기용 임시 버퍼 초기화 (오디오 스레드에서 재할당 방지)
        tempBuffer.setSize(numChannels, bufferSize / 4); // 일반적인 블록 크기보다 큰 값
    }

    /**
     * 마이크 데이터 쓰기 (오디오 스레드에서 호출)
     * @param inputChannelData 입력 채널 데이터 배열
     * @param numChannels 채널 수
     * @param numSamples 샘플 수
     */
    void writeMicrophone(const float* const* inputChannelData, int numChannels, int numSamples)
    {
        // 소스 타입 마이크로 설정
        currentSourceType = SourceType::Microphone;
        
        // 내부 쓰기 함수 호출 (마이크 FIFO에 쓰기)
        writeToFifo(micFifo, micBuffer, inputChannelData, numChannels, numSamples);
    }
    
    /**
     * 재생 데이터 쓰기 (오디오 스레드에서 호출)
     * @param inputChannelData 입력 채널 데이터 배열
     * @param numChannels 채널 수
     * @param numSamples 샘플 수
     */
    void writePlayback(const float* const* inputChannelData, int numChannels, int numSamples)
    {
        // 소스 타입 재생으로 설정
        currentSourceType = SourceType::Playback;
        
        // 내부 쓰기 함수 호출 (재생 FIFO에 쓰기)
        writeToFifo(playbackFifo, playbackBuffer, inputChannelData, numChannels, numSamples);
    }

    /**
     * 오디오 데이터 쓰기 - 하위 호환성 유지 (오디오 스레드에서 호출)
     * @param inputChannelData 입력 채널 데이터 배열
     * @param numChannels 채널 수
     * @param numSamples 샘플 수
     * @param sourceType 오디오 소스 유형 (마이크, 재생)
     */
    void write(const float* const* inputChannelData, int numChannels, int numSamples, 
              SourceType sourceType = SourceType::Microphone)
    {
        // 소스 타입에 따라 적절한 쓰기 함수 호출
        if (sourceType == SourceType::Microphone)
            writeMicrophone(inputChannelData, numChannels, numSamples);
        else if (sourceType == SourceType::Playback)
            writePlayback(inputChannelData, numChannels, numSamples);
    }

    /**
     * 마이크 데이터 읽기 (GUI 스레드에서 호출)
     * @param buffer 데이터를 읽어올 버퍼
     * @return 읽은 샘플 수
     */
    int readMicrophone(juce::AudioBuffer<float>& buffer)
    {
        return readFromFifo(micFifo, micBuffer, buffer);
    }
    
    /**
     * 재생 데이터 읽기 (GUI 스레드에서 호출)
     * @param buffer 데이터를 읽어올 버퍼
     * @return 읽은 샘플 수
     */
    int readPlayback(juce::AudioBuffer<float>& buffer)
    {
        return readFromFifo(playbackFifo, playbackBuffer, buffer);
    }

    /**
     * 오디오 데이터 읽기 - 하위 호환성 유지 (GUI 스레드에서 호출)
     * 현재 설정된 소스 타입에 따라 마이크 또는 재생 데이터를 읽음
     * @param buffer 데이터를 읽어올 버퍼
     * @return 읽은 샘플 수
     */
    int read(juce::AudioBuffer<float>& buffer)
    {
        // 현재 설정된 소스 타입에 따라 적절한 읽기 함수 호출
        if (currentSourceType == SourceType::Microphone)
            return readMicrophone(buffer);
        else if (currentSourceType == SourceType::Playback)
            return readPlayback(buffer);
        
        return 0; // 알 수 없는 소스 타입
    }
    
    /**
     * 특정 소스 타입의 데이터 읽기 (GUI 스레드에서 호출)
     * 현재 설정된 소스 타입과 관계없이 지정된 소스의 데이터만 읽음
     * @param buffer 데이터를 읽어올 버퍼
     * @param sourceType 읽을 소스 타입
     * @return 읽은 샘플 수
     */
    int readSource(juce::AudioBuffer<float>& buffer, SourceType sourceType)
    {
        if (sourceType == SourceType::Microphone)
            return readMicrophone(buffer);
        else if (sourceType == SourceType::Playback)
            return readPlayback(buffer);
        
        return 0; // 알 수 없는 소스 타입
    }

    /**
     * 현재 활성화된 오디오 소스 타입 반환
     * @return 현재 오디오 소스 타입
     */
    SourceType getSourceType() const {
        return currentSourceType;
    }

    /**
     * 원하는 오디오 소스 타입 설정
     * @param sourceType 설정할 오디오 소스 타입
     */
    void setSourceType(SourceType sourceType) {
        currentSourceType = sourceType;
    }

private:
    // 마이크 FIFO 및 버퍼
    juce::AbstractFifo micFifo;
    juce::AudioBuffer<float> micBuffer;
    
    // 재생 FIFO 및 버퍼
    juce::AbstractFifo playbackFifo;
    juce::AudioBuffer<float> playbackBuffer;
    
    // 데이터 읽기용 임시 버퍼 (GUI 스레드에서 사용)
    juce::AudioBuffer<float> tempBuffer;
    
    // 현재 활성화된 오디오 소스 타입
    SourceType currentSourceType;
    
    /**
     * 내부 쓰기 함수 - 지정된 FIFO와 버퍼에 데이터 쓰기
     * @param fifo 데이터를 쓸 FIFO
     * @param buffer 데이터를 저장할 버퍼
     * @param inputChannelData 입력 채널 데이터 배열
     * @param numChannels 채널 수
     * @param numSamples 샘플 수
     */
    void writeToFifo(juce::AbstractFifo& fifo, juce::AudioBuffer<float>& buffer,
                    const float* const* inputChannelData, int numChannels, int numSamples)
    {
        // 호출 빈도 조절 (모든 블록을 처리할 필요 없음)
        static int callCounter = 0;
        if ((++callCounter % 2) != 0) // 절반만 처리
            return;

        // 채널 수 확인 및 제한
        const int channelsToProcess = juce::jmin(numChannels, buffer.getNumChannels());
        if (channelsToProcess <= 0 || numSamples <= 0)
            return;

        // FIFO가 공간을 가지고 있는지 확인
        int start1, size1, start2, size2;
        fifo.prepareToWrite(numSamples, start1, size1, start2, size2);

        if (size1 + size2 < numSamples) // 충분한 공간이 없으면 무시
            return;

        // 첫 번째 블록 쓰기
        for (int ch = 0; ch < channelsToProcess; ++ch)
        {
            if (inputChannelData[ch] != nullptr)
            {
                // 데이터를 버퍼에 복사
                juce::FloatVectorOperations::copy(
                    buffer.getWritePointer(ch, start1),
                    inputChannelData[ch],
                    size1);

                // 두 번째 블록이 있다면 처리
                if (size2 > 0)
                {
                    juce::FloatVectorOperations::copy(
                        buffer.getWritePointer(ch, start2),
                        inputChannelData[ch] + size1,
                        size2);
                }
            }
        }

        // 쓰기 완료 통지
        fifo.finishedWrite(size1 + size2);
    }
    
    /**
     * 내부 읽기 함수 - 지정된 FIFO와 버퍼에서 데이터 읽기
     * @param fifo 데이터를 읽을 FIFO
     * @param buffer 데이터가 저장된 버퍼
     * @param outBuffer 읽은 데이터를 저장할 출력 버퍼
     * @return 읽은 샘플 수
     */
    int readFromFifo(juce::AbstractFifo& fifo, juce::AudioBuffer<float>& buffer,
                    juce::AudioBuffer<float>& outBuffer)
    {
        // 버퍼가 준비되었는지 확인
        auto numChannels = juce::jmin(outBuffer.getNumChannels(), buffer.getNumChannels());
        auto numSamples = juce::jmin(outBuffer.getNumSamples(), tempBuffer.getNumSamples());
        
        if (numChannels <= 0 || numSamples <= 0)
            return 0;

        // FIFO에서 읽을 수 있는 샘플 확인
        int start1, size1, start2, size2;
        fifo.prepareToRead(numSamples, start1, size1, start2, size2);

        if (size1 + size2 <= 0) // 읽을 데이터가 없으면 반환
            return 0;

        // 사용 가능한 샘플 수로 제한
        numSamples = size1 + size2;

        // 먼저 임시 버퍼로 복사
        for (int ch = 0; ch < numChannels; ++ch)
        {
            // 첫 번째 블록 복사
            juce::FloatVectorOperations::copy(
                tempBuffer.getWritePointer(ch),
                buffer.getReadPointer(ch, start1),
                size1);

            // 두 번째 블록이 있다면 복사
            if (size2 > 0)
            {
                juce::FloatVectorOperations::copy(
                    tempBuffer.getWritePointer(ch, size1),
                    buffer.getReadPointer(ch, start2),
                    size2);
            }
        }

        // 읽기 완료 통지
        fifo.finishedRead(size1 + size2);

        // 출력 버퍼로 복사
        for (int ch = 0; ch < numChannels; ++ch)
        {
            outBuffer.copyFrom(ch, 0, tempBuffer, ch, 0, numSamples);
        }

        return numSamples;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioTap)
}; 