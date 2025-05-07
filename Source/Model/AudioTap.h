#pragma once
#include <JuceHeader.h>

/**
 * AudioTap - 실시간 오디오 처리에 안전한 락프리 오디오 데이터 탭
 * 오디오 스레드에서 GUI 스레드로 안전하게 데이터를 전달하는 FIFO 기반 구현
 */
class AudioTap
{
public:
    /**
     * 생성자 - 버퍼 크기 및 채널 수 초기화
     * @param numChannels 지원할 채널 수 (기본 2)
     * @param bufferSize FIFO 버퍼 크기 (기본 16384)
     */
    AudioTap(int numChannels = 2, int bufferSize = 16384)
        : fifo(bufferSize), tapBuffer(numChannels, bufferSize)
    {
        // 초기화 시 버퍼 준비 (오디오 스레드에서 재할당 방지)
        tempBuffer.setSize(numChannels, bufferSize / 4); // 일반적인 블록 크기보다 큰 값
    }

    /**
     * 오디오 데이터 쓰기 (오디오 스레드에서 호출)
     * @param inputChannelData 입력 채널 데이터 배열
     * @param numChannels 채널 수
     * @param numSamples 샘플 수
     */
    void write(const float* const* inputChannelData, int numChannels, int numSamples)
    {
        // 호출 빈도 조절 (모든 블록을 처리할 필요 없음)
        static int callCounter = 0;
        if ((++callCounter % 2) != 0) // 절반만 처리
            return;

        // 채널 수 확인 및 제한
        const int channelsToProcess = juce::jmin(numChannels, tapBuffer.getNumChannels());
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
                // 데이터를 tapBuffer에 복사
                juce::FloatVectorOperations::copy(
                    tapBuffer.getWritePointer(ch, start1),
                    inputChannelData[ch],
                    size1);

                // 두 번째 블록이 있다면 처리
                if (size2 > 0)
                {
                    juce::FloatVectorOperations::copy(
                        tapBuffer.getWritePointer(ch, start2),
                        inputChannelData[ch] + size1,
                        size2);
                }
            }
        }

        // 쓰기 완료 통지
        fifo.finishedWrite(size1 + size2);
    }

    /**
     * 오디오 데이터 읽기 (GUI 스레드에서 호출)
     * @param buffer 데이터를 읽어올 버퍼
     * @return 읽은 샘플 수
     */
    int read(juce::AudioBuffer<float>& buffer)
    {
        // 버퍼가 준비되었는지 확인
        auto numChannels = juce::jmin(buffer.getNumChannels(), tapBuffer.getNumChannels());
        auto numSamples = juce::jmin(buffer.getNumSamples(), tempBuffer.getNumSamples());
        
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
                tapBuffer.getReadPointer(ch, start1),
                size1);

            // 두 번째 블록이 있다면 복사
            if (size2 > 0)
            {
                juce::FloatVectorOperations::copy(
                    tempBuffer.getWritePointer(ch, size1),
                    tapBuffer.getReadPointer(ch, start2),
                    size2);
            }
        }

        // 읽기 완료 통지
        fifo.finishedRead(size1 + size2);

        // 출력 버퍼로 복사
        for (int ch = 0; ch < numChannels; ++ch)
        {
            buffer.copyFrom(ch, 0, tempBuffer, ch, 0, numSamples);
        }

        return numSamples;
    }

private:
    juce::AbstractFifo fifo;
    juce::AudioBuffer<float> tapBuffer;
    juce::AudioBuffer<float> tempBuffer; // 읽기용 임시 버퍼 (GUI 스레드에서 사용)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioTap)
}; 