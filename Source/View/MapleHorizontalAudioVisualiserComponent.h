#pragma once
#include <JuceHeader.h>

/**
 * 마이크 입력 오디오 데이터를 수평 주파수 바로 시각화하는 컴포넌트
 * 가로 방향 주파수 바를 표시합니다.
 */
class MapleHorizontalAudioVisualiserComponent : public juce::Component,
                                              public juce::Timer
{
public:
    MapleHorizontalAudioVisualiserComponent()
    {
        // 색상 팔레트 초기화 - 더 아름다운 그래디언트를 위한 색상 선택
        colours.add(juce::Colour(0xff1E3FFF)); // 진한 파랑 (저주파 영역)
        colours.add(juce::Colour(0xff00A4FF)); // 밝은 파랑 (낮은 중주파)
        colours.add(juce::Colour(0xff00FFBF)); // 청록색 (중간 주파수 - 기타 핵심 영역)
        colours.add(juce::Colour(0xffFFD500)); // 노랑/금색 (높은 중주파)
        colours.add(juce::Colour(0xffFF5C00)); // 주황/오렌지 (고주파)
        
        // 그래디언트를 위한 추가 색상 팔레트
        // 중간 블렌딩 색상을 추가하여 더 부드러운 전환 제공
        gradientColours.add(juce::Colour(0xff1E3FFF)); // 진한 파랑
        gradientColours.add(juce::Colour(0xff0069FF)); // 중간 파랑
        gradientColours.add(juce::Colour(0xff00A4FF)); // 밝은 파랑
        gradientColours.add(juce::Colour(0xff00D9FF)); // 하늘색
        gradientColours.add(juce::Colour(0xff00FFBF)); // 청록색
        gradientColours.add(juce::Colour(0xff80FF00)); // 라임색
        gradientColours.add(juce::Colour(0xffFFD500)); // 노랑/금색
        gradientColours.add(juce::Colour(0xffFF8A00)); // 금색/주황
        gradientColours.add(juce::Colour(0xffFF5C00)); // 주황/오렌지
        
        // FFT 초기화
        fftSize = 1024;
        fftEngine = std::make_unique<juce::dsp::FFT>(std::log2(fftSize));
        fftData.resize(fftSize * 2, 0.0f);
        fifoIndex = 0;
        fftDataFifo.resize(fftSize, 0.0f);
        
        // 주파수 밴드 설정 - 더 연속적인 시각화를 위해 밴드 수 증가
        numBands = 256; // 밴드 수 증가 (기존 128에서 256으로)
        
        // 스펙트럼 데이터 초기화
        for (int i = 0; i < numBands; ++i)
            frequencyData.add(0.0f);
            
        // 이동 평균 필터 히스토리 초기화 - 더 부드러운 전환을 위해 사이즈 증가
        for (int i = 0; i < movingAverageSize; ++i) {
            juce::Array<float> history;
            for (int j = 0; j < numBands; ++j) {
                history.add(0.0f);
            }
            frequencyHistory.add(history);
        }
        
        // 스무스 값 초기화
        smoothedFrequencyData.resize(numBands, 0.0f);
        
        // 보간된 데이터 저장용 (중간 값 포함)
        interpolatedData.resize(numBands, 0.0f);
            
        // 파형 데이터 초기화
        waveformData.setSize(2, 1024);
        
        // 밴드별 인덱스 캐싱 (FFT 계산 최적화)
        bandIndexCache.resize(numBands);
        
        // 밴드 인덱스 미리 계산
        precomputeBandIndices();
        
        // 타이머 시작 - 더 부드러운 애니메이션을 위해 주파수 증가
        startTimerHz(60);
        
        // 마지막 페인트 시간 초기화
        lastPaintTime = juce::Time::getMillisecondCounterHiRes();
        
        // 불투명 설정 (렌더링 최적화)
        setOpaque(true);
        
        // 기타 주파수 특화 설정
        dynamicScaleFactor = 270.0f; // 진폭 스케일 증가
        
        // 처음부터 보간 모드 활성화
        useInterpolation = true;
        useGradientRendering = true;
        
        // 시각화 모드 초기화
        visualisationMode = GradientSpectrum;
    }
    
    ~MapleHorizontalAudioVisualiserComponent() override
    {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override
    {
        // 현재 시간 가져오기 (프레임 간 시간 계산용)
        double currentTime = juce::Time::getMillisecondCounterHiRes();
        double deltaTime = currentTime - lastPaintTime;
        lastPaintTime = currentTime;
        
        // 애니메이션 속도를 일정하게 유지
        animationOffset += 0.03f * juce::jlimit(0.5f, 2.0f, (float)(deltaTime / 16.67));
        
        auto bounds = getLocalBounds().toFloat();
        
        // 배경 그리기 (어두운 색상)
        g.fillAll(juce::Colour(0xff151515));
        
        // 수평 주파수 바 그리기
        drawHorizontalBars(g, bounds);
    }
    
    void resized() override
    {
        // 크기가 변경될 때 시각화 파라미터 업데이트
        updateVisualizationParams();
        
        // 화면 크기에 따라 최적화된 설정
        const int width = getWidth();
        if (width < 400)
        {
            // 작은 화면에서는 더 적은 밴드 사용
            numBands = 32;
            // 데이터 초기화
            updateDataStructures();
        }
        else if (width > 800)
        {
            // 큰 화면에서는 더 많은 밴드 사용
            numBands = 64;
            // 데이터 초기화
            updateDataStructures();
        }
    }
    
    void timerCallback() override
    {
        // audioDataChanged가 true일 때만 repaint 호출
        if (audioDataChanged)
        {
            repaint();
            audioDataChanged = false;
        }
        else
        {
            // 데이터가 변경되지 않았더라도 애니메이션 진행
            if (animationOffset > 1000.0f)
                animationOffset = 0.0f;
                
            // 낮은 빈도로 repaint 호출 (애니메이션 유지)
            if (++idleFrameCount > 2)
            {
                repaint();
                idleFrameCount = 0;
            }
        }
    }
    
    // 오디오 데이터 설정
    void pushBuffer(const juce::AudioBuffer<float>& buffer)
    {
        const juce::ScopedLock sl(audioDataLock);
        
        // 버퍼 복사
        auto numChannels = juce::jmin(buffer.getNumChannels(), 2);
        auto numSamples = buffer.getNumSamples();
        
        // 파형 표시용 데이터 저장
        if (waveformData.getNumChannels() != numChannels || 
            waveformData.getNumSamples() != numSamples)
        {
            waveformData.setSize(numChannels, numSamples);
        }
        
        // 데이터 복사
        for (int ch = 0; ch < numChannels; ++ch)
            waveformData.copyFrom(ch, 0, buffer, ch, 0, numSamples);
            
        // FFT 데이터 처리
        if (numChannels > 0)
        {
            const float* samples = buffer.getReadPointer(0);
            
            // 샘플링 간격 계산 (최적화)
            int stride = numSamples > 1024 ? numSamples / 1024 : 1;
            
            // 처리 시간 측정
            auto startTime = juce::Time::getMillisecondCounterHiRes();
            
            // 샘플 처리
            for (int i = 0; i < numSamples; i += stride)
            {
                // FFT용 FIFO에 샘플 추가 
                pushNextSampleIntoFifo(samples[i]);
                
                // FFT 계산 빈도 제한 (성능 최적화)
                double elapsedMs = juce::Time::getMillisecondCounterHiRes() - startTime;
                if (elapsedMs > 4.0) // 4ms 이상 소요되면 중단
                    break;
            }
        }
        
        audioDataChanged = true;
    }
    
    // 주파수 범위 설정 메서드 추가
    void setFrequencyRange(float minFrequency, float maxFrequency)
    {
        this->minFrequency = minFrequency;
        this->maxFrequency = maxFrequency;
        
        // 밴드 인덱스 다시 계산
        precomputeBandIndices();
    }
    
    // 진폭 스케일 팩터 설정 메서드 추가
    void setDynamicScaleFactor(float factor)
    {
        dynamicScaleFactor = factor;
    }
    
    // 현재 진폭 스케일 팩터 반환
    float getDynamicScaleFactor() const
    {
        return dynamicScaleFactor;
    }
    
    // 시각화 모드 설정 열거형
    enum VisualisationMode
    {
        GradientSpectrum,   // 연속적인 그래디언트 스펙트럼
        BarSpectrum         // 개별 막대 스펙트럼
    };
    
    // 시각화 모드 설정 메서드
    void setVisualisationMode(VisualisationMode newMode)
    {
        visualisationMode = newMode;
        repaint();
    }
    
    // 현재 시각화 모드 반환
    VisualisationMode getVisualisationMode() const
    {
        return visualisationMode;
    }
    
    // 시각화 모드 전환 메서드
    void toggleVisualisationMode()
    {
        if (visualisationMode == GradientSpectrum)
            visualisationMode = BarSpectrum;
        else
            visualisationMode = GradientSpectrum;
        
        repaint();
    }
    
private:
    // 데이터 구조 업데이트 (밴드 수 변경 시 호출)
    void updateDataStructures()
    {
        // 밴드 인덱스 캐시 크기 조정
        bandIndexCache.resize(numBands);
        
        // 주파수 데이터 초기화
        frequencyData.clearQuick();
        for (int i = 0; i < numBands; ++i)
            frequencyData.add(0.0f);
            
        // 스무스 데이터 초기화
        smoothedFrequencyData.resize(numBands, 0.0f);
        
        // 이동 평균 히스토리 초기화
        frequencyHistory.clearQuick();
        for (int i = 0; i < movingAverageSize; ++i) {
            juce::Array<float> history;
            for (int j = 0; j < numBands; ++j) {
                history.add(0.0f);
            }
            frequencyHistory.add(history);
        }
        
        // 밴드 인덱스 미리 계산
        precomputeBandIndices();
    }
    
    // FFT 밴드 인덱스 미리 계산 (성능 최적화)
    void precomputeBandIndices()
    {
        // 멤버 변수의 기타 주파수 범위 사용 (80Hz ~ 2000Hz)
        // 이 범위는 setFrequencyRange를 통해 조정할 수 있음
        
        for (int band = 0; band < numBands; ++band)
        {
            // 로그 스케일로 밴드 인덱스 계산
            float startFraction, endFraction;
            
            if (useLogFrequencyScale)
            {
                // 로그 스케일로 기타 주파수 범위 내에서 밴드 계산
                float logMinFreq = std::log(minFrequency);
                float logMaxFreq = std::log(maxFrequency);
                float logFreqRange = logMaxFreq - logMinFreq;
                
                // 밴드별 주파수 범위 계산 (로그 스케일)
                float bandLogStart = logMinFreq + (band * logFreqRange) / numBands;
                float bandLogEnd = logMinFreq + ((band + 1) * logFreqRange) / numBands;
                
                // 주파수를 FFT 인덱스로 변환
                float sampleRate = 44100.0f; // 기본 샘플레이트 가정
                
                float startFreq = std::exp(bandLogStart);
                float endFreq = std::exp(bandLogEnd);
                
                startFraction = startFreq * fftSize / sampleRate;
                endFraction = endFreq * fftSize / sampleRate;
            }
            else
            {
                // 선형 스케일로 기타 주파수 범위 내에서 밴드 계산
                float freqRange = maxFrequency - minFrequency;
                float startFreq = minFrequency + (band * freqRange) / numBands;
                float endFreq = minFrequency + ((band + 1) * freqRange) / numBands;
                
                // 주파수를 FFT 인덱스로 변환
                float sampleRate = 44100.0f; // 기본 샘플레이트 가정
                
                startFraction = startFreq * fftSize / sampleRate;
                endFraction = endFreq * fftSize / sampleRate;
            }
            
            // 인덱스 범위 캐싱 (유효한 범위 내로 제한)
            bandIndexCache[band] = std::make_pair(
                juce::jlimit(1, fftSize / 2 - 1, (int)startFraction),
                juce::jlimit(1, fftSize / 2 - 1, (int)endFraction)
            );
        }
    }
    
    // FIFO에 샘플 추가
    void pushNextSampleIntoFifo(float sample)
    {
        // FIFO 인덱스 순환
        if (fifoIndex == fftSize)
        {
            // FIFO가 가득 차면 FFT 계산
            calculateFFT();
            fifoIndex = 0;
        }
        
        // 샘플 추가
        fftDataFifo[fifoIndex++] = sample;
    }
    
    // FFT 계산
    void calculateFFT()
    {
        // 한 번에 너무 자주 FFT를 계산하지 않도록 제한 (성능 최적화)
        double currentTimeMs = juce::Time::getMillisecondCounterHiRes();
        if (currentTimeMs - lastFftTimeMs < 16.0) // 약 60fps 제한
            return;
            
        lastFftTimeMs = currentTimeMs;
        
        // FIFO에서 FFT 데이터 복사
        std::fill(fftData.begin(), fftData.end(), 0.0f);
        
        // 윈도잉 함수 적용 (해닝 윈도우)
        for (int i = 0; i < fftSize; ++i)
        {
            // 해닝 윈도우 계산
            float window = 0.5f - 0.5f * std::cos(2.0f * juce::MathConstants<float>::pi * i / (fftSize - 1));
            fftData[i * 2] = fftDataFifo[i] * window;
        }
        
        // FFT 계산
        fftEngine->performFrequencyOnlyForwardTransform(fftData.data());
        
        // 결과 처리
        juce::Array<float> newFrequencyData;
        for (int i = 0; i < numBands; ++i)
            newFrequencyData.add(0.0f);
            
        // 각 밴드의 에너지 계산
        for (int band = 0; band < numBands; ++band)
        {
            const auto& range = bandIndexCache[band];
            int startIdx = range.first;
            int endIdx = range.second;
            
            float sum = 0.0f;
            int count = 0;
            
            for (int fftBin = startIdx; fftBin < endIdx; ++fftBin)
            {
                sum += fftData[fftBin];
                count++;
            }
            
            // 평균 에너지 계산
            float avg = count > 0 ? sum / count : 0.0f;
            
            // 스케일링 및 정규화
            avg = avg * dynamicScaleFactor;
            
            // 기타 주파수 대역에 따른 가중치 적용 (기타 음색 최적화)
            float weight = 1.0f;
            float normalizedBand = (float)band / numBands;
            
            // 기타 주파수 대역별 가중치 설정
            // 낮은 주파수(80-300Hz): 베이스 줄 강조
            // 중간 주파수(300-1200Hz): 기타 주요 음역 최대 강조
            // 높은 주파수(1200-2000Hz): 하모닉스 적당히 강조
            if (normalizedBand < 0.2f) {
                // 저주파수 영역 (베이스 줄 - 기타 E, A, D 줄 등)
                weight = 1.1f + 0.2f * normalizedBand * 5.0f; // 0.2까지 1.1~1.5 사이 증가
            } 
            else if (normalizedBand >= 0.2f && normalizedBand < 0.6f) {
                // 중간 주파수 영역 (기타의 주요 음역 - G, B, E 줄 등)
                // 사인 곡선으로 중간에 최대값을 갖도록 설정
                weight = 1.6f + 0.6f * std::sin((normalizedBand - 0.2f) / 0.4f * juce::MathConstants<float>::pi);
            }
            else {
                // 고주파수 영역 (하모닉스 등)
                weight = 1.4f - 0.7f * (normalizedBand - 0.6f) / 0.4f; // 0.6부터 1.4에서 0.7로 감소
            }
                
            avg *= weight;
            
            // 결과 저장
            newFrequencyData.set(band, juce::jlimit(0.0f, 1.0f, avg));
        }
        
        // 시각적 효과를 위한 스무딩 적용
        updateSmoothedFrequency(newFrequencyData);
    }
    
    // 주파수 데이터 스무딩
    void updateSmoothedFrequency(const juce::Array<float>& newData)
    {
        // 전역 변수
        float minThreshold = 0.005f; // 최소 임계값 (노이즈 제거)
        
        // 이동 평균 업데이트
        for (int i = 0; i < movingAverageSize - 1; ++i) {
            frequencyHistory.getReference(i) = frequencyHistory.getReference(i + 1);
        }
        frequencyHistory.getReference(movingAverageSize - 1) = newData;
        
        // 이동 평균 계산 및 스무딩 적용
        for (int band = 0; band < numBands; ++band)
        {
            // 이동 평균 계산
            float sum = 0.0f;
            for (int i = 0; i < movingAverageSize; ++i) {
                sum += frequencyHistory.getReference(i).getReference(band);
            }
            float avg = sum / movingAverageSize;
            
            // 이전 값과 현재 값 사이의 부드러운 전이
            // 새 값이 더 높을 때는 빠르게 상승, 낮을 때는 천천히 감소
            float smoothingFactor = avg > smoothedFrequencyData[band] ? 0.7f : 0.3f;
            
            // 활성 주파수에만 반응하기 위한 디케이 효과 추가
            if (avg < minThreshold && smoothedFrequencyData[band] > 0.0f)
            {
                // 디케이 효과 적용
                smoothedFrequencyData[band] = smoothedFrequencyData[band] * 0.85f;
                if (smoothedFrequencyData[band] < minThreshold)
                    smoothedFrequencyData[band] = 0.0f;
            }
            else
            {
                // 정상적인 스무딩
                smoothedFrequencyData[band] = smoothedFrequencyData[band] * (1.0f - smoothingFactor) + avg * smoothingFactor;
                
                // 전역 감쇠 적용 (자동 감소)
                if (globalDecayFactor > 0.05f)
                    smoothedFrequencyData[band] *= (1.02f - globalDecayFactor);
            }
            
            // 주파수 값에 한계 설정
            smoothedFrequencyData[band] = juce::jmin(0.98f, smoothedFrequencyData[band]);
        }
        
        // 스무딩된 데이터를 보간하여 더 연속적인 스펙트럼 생성
        if (useInterpolation)
            interpolateFrequencyData();
    }
    
    // 주파수 데이터 보간 - 빈 공간 채우기와 연속적인 스펙트럼을 위한 메서드
    void interpolateFrequencyData()
    {
        // 보간 전 기존 데이터 복사
        for (int band = 0; band < numBands; ++band)
            interpolatedData[band] = smoothedFrequencyData[band];
            
        // 보간 적용 (0이거나 매우 낮은 값을 주변 값에 기반하여 보정)
        for (int band = 1; band < numBands - 1; ++band)
        {
            // 현재 값이 매우 낮고, 주변 값이 높다면 보간 적용
            float currentValue = interpolatedData[band];
            if (currentValue < 0.01f) // 낮은 값 기준점
            {
                float prevValue = interpolatedData[band - 1];
                float nextValue = interpolatedData[band + 1];
                
                // 양 옆의 값이 모두 일정 기준 이상일 때만 보간
                if (prevValue > 0.05f || nextValue > 0.05f)
                {
                    // 양쪽 값의 가중 평균으로 보간 (가까운 쪽에 더 큰 가중치)
                    float interpolated = 0.0f;
                    float weightSum = 0.0f;
                    
                    if (prevValue > 0.01f)
                    {
                        float weight = 1.0f;
                        interpolated += prevValue * weight;
                        weightSum += weight;
                    }
                    
                    if (nextValue > 0.01f)
                    {
                        float weight = 1.0f;
                        interpolated += nextValue * weight;
                        weightSum += weight;
                    }
                    
                    if (weightSum > 0.0f)
                    {
                        // 실제 가중 평균 계산 및 주변 값보다 약간 낮게 설정
                        interpolated = (interpolated / weightSum) * 0.85f;
                        interpolatedData[band] = interpolated;
                    }
                }
            }
        }
        
        // 추가 스무딩 - 인접 밴드 간 부드러운 전환을 위한 가우시안 블러 효과
        std::vector<float> tempData = interpolatedData;
        
        for (int band = 2; band < numBands - 2; ++band)
        {
            // 5-포인트 가우시안 블러 커널 적용 (1,4,6,4,1)/16
            float blurred = (
                interpolatedData[band - 2] * 1.0f +
                interpolatedData[band - 1] * 4.0f +
                interpolatedData[band]     * 6.0f +
                interpolatedData[band + 1] * 4.0f +
                interpolatedData[band + 2] * 1.0f
            ) / 16.0f;
            
            // 원래 값과 블러된 값 사이 적절히 블렌딩
            float blendFactor = 0.3f; // 블러 강도 (0.0 ~ 1.0)
            tempData[band] = interpolatedData[band] * (1.0f - blendFactor) + blurred * blendFactor;
        }
        
        // 블러 결과를 다시 원래 배열에 복사
        interpolatedData = tempData;
    }
    
    // 수평 주파수 바 그리기
    void drawHorizontalBars(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const juce::ScopedLock sl(audioDataLock);
        
        // 배경에 주파수 영역 표시
        drawFrequencyRangeBackground(g, bounds, bounds.getBottom() - 10.0f);
        
        // 시각화 모드에 따른 렌더링 선택
        switch (visualisationMode)
        {
            case GradientSpectrum:
                drawGradientSpectrum(g, bounds);
                break;
                
            case BarSpectrum:
                drawBarSpectrum(g, bounds);
                break;
                
            default:
                drawGradientSpectrum(g, bounds);
                break;
        }
    }
    
    // 막대 스펙트럼 그리기 - 개별 막대 시각화
    void drawBarSpectrum(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        float maxHeight = bounds.getHeight() * 0.85f; // 화면 높이의 85%를 최대 높이로 설정
        float baseY = bounds.getBottom() - 10.0f; // 약간의 여백
        
        // 보간된 데이터 사용
        const std::vector<float>& dataToUse = useInterpolation ? interpolatedData : smoothedFrequencyData;
        
        // 막대 간격 설정 - 더 좁은 간격으로 연속적인 느낌 부여
        int dividerInterval = 1; // 각 밴드마다 하나의 막대 (최대한 조밀하게)
        
        // 주파수 막대 그리기
        for (int band = 0; band < numBands; band += dividerInterval)
        {
            float x = bounds.getX() + (float)band * bounds.getWidth() / numBands;
            float lineHeight = 0.0f;
            
            // 현재 밴드의 값과 다음 밴드의 값 중 큰 값으로 라인 높이 결정
            float value = band < dataToUse.size() ? dataToUse[band] : 0.0f;
            
            // 밴드 범위(band에서 band+dividerInterval)에서 최대값 찾기
            float maxValue = value;
            for (int i = 1; i < dividerInterval && (band + i) < dataToUse.size(); ++i) {
                maxValue = juce::jmax(maxValue, dataToUse[band + i]);
            }
            
            lineHeight = maxValue * maxHeight;
            
            // 최소 높이 보장 (베이스라인 위로 약간 올라오게)
            lineHeight = juce::jmax(lineHeight, 2.0f);
            
            // 알파값 계산 (진폭에 따라 더 밝게)
            float alpha = 0.15f + 0.25f * lineHeight / maxHeight;
            
            // 주파수 대역에 따른 색상 선택
            float normalizedBand = (float)band / numBands;
            juce::Colour dividerColour;
            
            // 주파수 영역별 색상 맵핑
            if (normalizedBand < 0.2f) {
                dividerColour = gradientColours[0].brighter(0.8f);
            } else if (normalizedBand < 0.4f) {
                dividerColour = gradientColours[2].brighter(0.8f);
            } else if (normalizedBand < 0.6f) {
                dividerColour = gradientColours[4].brighter(0.8f);
            } else if (normalizedBand < 0.8f) {
                dividerColour = gradientColours[6].brighter(0.8f);
            } else {
                dividerColour = gradientColours[8].brighter(0.8f);
            }
            
            // 막대의 오른쪽 경계 계산
            float xEnd = bounds.getX() + (float)(band + dividerInterval) * bounds.getWidth() / numBands;
            if (xEnd > bounds.getRight()) xEnd = bounds.getRight();
            
            // 바 너비 계산 (더 얇게 만들기 위해 조정)
            float barWidth = (xEnd - x);
            float barPadding = barWidth * 0.05f; // 패딩 감소 (5%만 남김)
            float actualBarWidth = barWidth - barPadding;
            float barX = x + barPadding / 2;
            
            // 매우 얇은 막대로 그려서 연속적인 스펙트럼처럼 보이게 함
            // 막대 영역 채우기 - 반투명 채우기로 막대 효과 생성
            g.setColour(dividerColour.withAlpha(0.1f + 0.2f * lineHeight / maxHeight));
            g.fillRect(barX, baseY - lineHeight, actualBarWidth, lineHeight);
            
            // 막대 테두리 그리기 (더 얇은 선으로)
            g.setColour(dividerColour.withAlpha(alpha));
            // 왼쪽 경계
            g.drawLine(barX, baseY, barX, baseY - lineHeight, 0.3f);
            // 오른쪽 경계
            g.drawLine(barX + actualBarWidth, baseY, barX + actualBarWidth, baseY - lineHeight, 0.3f);
            // 상단 경계
            g.drawLine(barX, baseY - lineHeight, barX + actualBarWidth, baseY - lineHeight, 0.3f);
            
            // 막대 상단에 하이라이트 추가 (진폭이 높을 때만)
            if (lineHeight > maxHeight * 0.25f) {
                // 상단 하이라이트
                g.setColour(dividerColour.brighter(0.5f).withAlpha(0.4f + 0.3f * lineHeight / maxHeight));
                g.drawLine(barX, baseY - lineHeight + 0.5f, barX + actualBarWidth, baseY - lineHeight + 0.5f, 0.7f);
                
                // 상단 점 효과 (매우 높은 진폭에서만)
                if (lineHeight > maxHeight * 0.6f) {
                    float centerX = barX + actualBarWidth * 0.5f;
                    float dotSize = 1.5f;
                    g.setColour(dividerColour.brighter(1.0f).withAlpha(0.8f));
                    g.fillEllipse(centerX - dotSize/2, baseY - lineHeight - dotSize/2, dotSize, dotSize);
                }
            }
        }
        
        // 주요 주파수 대역 구분선 (더 강조된 구분선)
        // 주요 주파수 영역 경계 계산 (기타 주요 주파수 영역)
        float lowMidBound = bounds.getX() + bounds.getWidth() * 0.2f;  // 300Hz 지점
        float midHighBound = bounds.getX() + bounds.getWidth() * 0.6f; // 1200Hz 지점
        
        // 주요 구분선 그리기
        g.setColour(juce::Colours::white.withAlpha(0.1f)); // 더 투명하게
        g.drawLine(lowMidBound, baseY, lowMidBound, bounds.getY(), 0.5f); // 더 얇게
        g.drawLine(midHighBound, baseY, midHighBound, bounds.getY(), 0.5f);
    }
    
    // 그래디언트 스펙트럼 그리기 - 연속적인 스펙트럼 시각화
    void drawGradientSpectrum(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        float maxHeight = bounds.getHeight() * 0.85f; // 화면 높이의 85%를 최대 높이로 설정
        float baseY = bounds.getBottom() - 10.0f; // 약간의 여백
        
        // 보간된 데이터 사용
        const std::vector<float>& dataToUse = useInterpolation ? interpolatedData : smoothedFrequencyData;
        
        // 스펙트럼 경로 생성
        juce::Path spectrumPath;
        spectrumPath.startNewSubPath(bounds.getX(), baseY); // 왼쪽 하단에서 시작
        
        // 각 포인트를 경로에 추가
        for (int band = 0; band < numBands; ++band)
        {
            float x = bounds.getX() + (float)band * bounds.getWidth() / numBands;
            float value = band < dataToUse.size() ? dataToUse[band] : 0.0f;
            float y = baseY - value * maxHeight;
            
            // 서브패스가 시작되지 않았으면 시작, 그렇지 않으면 라인 추가
            if (band == 0)
                spectrumPath.startNewSubPath(x, y);
            else
                spectrumPath.lineTo(x, y);
        }
        
        // 경로 닫기 (오른쪽 하단 -> 왼쪽 하단)
        spectrumPath.lineTo(bounds.getRight(), baseY);
        spectrumPath.lineTo(bounds.getX(), baseY);
        spectrumPath.closeSubPath();
        
        // 그래디언트 컬러 맵 생성 (스펙트럼 포지션에 맞춰 색상 배치)
        juce::ColourGradient gradient;
        gradient.clearColours();
        
        int numColours = gradientColours.size();
        for (int i = 0; i < numColours; ++i)
        {
            float position = (float)i / (numColours - 1);
            gradient.addColour(position, gradientColours[i]);
        }
        
        // 애니메이션 효과를 위한 알파 맵 생성
        juce::Path spectrumOutline;
        spectrumOutline = spectrumPath;
        
        // 다중 그래디언트 렌더링
        // 1. 메인 채우기 - 부드러운 그래디언트
        juce::ColourGradient fillGradient(
            juce::Colour(0xff1E3FFF), bounds.getX(), baseY, // 왼쪽 하단
            juce::Colour(0xffFF5C00), bounds.getRight(), baseY, // 오른쪽 하단
            false); // 수직 그래디언트 아님
            
        // 그래디언트에 중간 색상 추가
        fillGradient.clearColours();
        for (int i = 0; i < gradientColours.size(); ++i)
        {
            float pos = (float)i / (gradientColours.size() - 1);
            fillGradient.addColour(pos, gradientColours[i].withAlpha(0.85f));
        }
        
        // 스펙트럼 채우기
        g.setGradientFill(fillGradient);
        g.fillPath(spectrumPath);
        
        // 2. 글로우 효과 
        juce::Path glowPath = spectrumPath;
        float glowThickness = 2.5f;
        juce::PathStrokeType strokeType(glowThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        
        juce::ColourGradient glowGradient(
            juce::Colour(0xff1E3FFF).brighter(0.7f), bounds.getX(), baseY, // 왼쪽 하단
            juce::Colour(0xffFF5C00).brighter(0.7f), bounds.getRight(), baseY, // 오른쪽 하단
            false); // 수직 그래디언트 아님
            
        glowGradient.clearColours();
        for (int i = 0; i < gradientColours.size(); ++i)
        {
            float pos = (float)i / (gradientColours.size() - 1);
            glowGradient.addColour(pos, gradientColours[i].brighter(0.8f).withAlpha(0.7f));
        }
        
        // 스톡을 위한 임시 경로 - 채워진 경로의 상단 가장자리만 포함
        juce::Path topEdgePath;
        topEdgePath.startNewSubPath(bounds.getX(), baseY); // 왼쪽 하단에서 시작
        
        // 각 포인트를 경로에 추가
        for (int band = 0; band < numBands; ++band)
        {
            float x = bounds.getX() + (float)band * bounds.getWidth() / numBands;
            float value = band < dataToUse.size() ? dataToUse[band] : 0.0f;
            float y = baseY - value * maxHeight;
            
            // 서브패스가 시작되지 않았으면 시작, 그렇지 않으면 라인 추가
            if (band == 0)
                topEdgePath.startNewSubPath(x, y);
            else
                topEdgePath.lineTo(x, y);
        }
        
        // 글로우 효과 적용
        g.setGradientFill(glowGradient);
        g.strokePath(topEdgePath, strokeType);
        
        // 3. 상단 엣지 하이라이트 
        juce::Path highlightPath = topEdgePath;
        
        juce::ColourGradient highlightGradient(
            juce::Colour(0xff1E3FFF).brighter(1.5f), bounds.getX(), baseY, // 왼쪽 하단
            juce::Colour(0xffFF5C00).brighter(1.5f), bounds.getRight(), baseY, // 오른쪽 하단
            false); // 수직 그래디언트 아님
            
        highlightGradient.clearColours();
        for (int i = 0; i < gradientColours.size(); ++i)
        {
            float pos = (float)i / (gradientColours.size() - 1);
            highlightGradient.addColour(pos, gradientColours[i].brighter(1.5f).withAlpha(0.9f + 
                                                    0.1f * std::sin(animationOffset * 0.1f + i * 0.3f)));
        }
        
        // 하이라이트 효과 적용 (얇은 라인)
        g.setGradientFill(highlightGradient);
        g.strokePath(topEdgePath, juce::PathStrokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // 바닥 라인 (베이스라인)
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawHorizontalLine((int)baseY, bounds.getX(), bounds.getRight());
        
        // 4. 막대 형태로 주파수 시각화
        int dividerInterval = 4; // 4개 밴드마다 구분선 표시 (더 조밀하게)
        
        // 주파수 막대 그리기
        for (int band = 0; band < numBands; band += dividerInterval)
        {
            float x = bounds.getX() + (float)band * bounds.getWidth() / numBands;
            float lineHeight = 0.0f;
            
            // 현재 밴드의 값과 다음 밴드의 값 중 큰 값으로 라인 높이 결정
            float value = band < dataToUse.size() ? dataToUse[band] : 0.0f;
            
            // 밴드 범위(band에서 band+dividerInterval)에서 최대값 찾기
            float maxValue = value;
            for (int i = 1; i < dividerInterval && (band + i) < dataToUse.size(); ++i) {
                maxValue = juce::jmax(maxValue, dataToUse[band + i]);
            }
            
            lineHeight = maxValue * maxHeight;
            
            // 최소 높이 보장 (베이스라인 위로 약간 올라오게)
            lineHeight = juce::jmax(lineHeight, 3.0f);
            
            // 알파값 계산 (진폭에 따라 더 밝게)
            float alpha = 0.15f + 0.25f * lineHeight / maxHeight;
            
            // 주파수 대역에 따른 색상 선택
            float normalizedBand = (float)band / numBands;
            juce::Colour dividerColour;
            
            // 주파수 영역별 색상 맵핑
            if (normalizedBand < 0.2f) {
                dividerColour = gradientColours[0].brighter(0.8f);
            } else if (normalizedBand < 0.4f) {
                dividerColour = gradientColours[2].brighter(0.8f);
            } else if (normalizedBand < 0.6f) {
                dividerColour = gradientColours[4].brighter(0.8f);
            } else if (normalizedBand < 0.8f) {
                dividerColour = gradientColours[6].brighter(0.8f);
            } else {
                dividerColour = gradientColours[8].brighter(0.8f);
            }
            
            // 막대의 오른쪽 경계 계산
            float xEnd = bounds.getX() + (float)(band + dividerInterval) * bounds.getWidth() / numBands;
            if (xEnd > bounds.getRight()) xEnd = bounds.getRight();
            
            // 막대 영역 채우기 - 반투명 채우기로 막대 효과 생성
            g.setColour(dividerColour.withAlpha(0.1f + 0.2f * lineHeight / maxHeight));
            g.fillRect(x, baseY - lineHeight, xEnd - x, lineHeight);
            
            // 막대 테두리 그리기
            g.setColour(dividerColour.withAlpha(alpha));
            // 왼쪽 경계
            g.drawLine(x, baseY, x, baseY - lineHeight, 0.6f);
            // 오른쪽 경계
            g.drawLine(xEnd, baseY, xEnd, baseY - lineHeight, 0.6f);
            // 상단 경계
            g.drawLine(x, baseY - lineHeight, xEnd, baseY - lineHeight, 0.5f);
            
            // 막대 상단에 하이라이트 추가 (진폭이 높을 때만)
            if (lineHeight > maxHeight * 0.25f) {
                // 상단 하이라이트
                g.setColour(dividerColour.brighter(0.5f).withAlpha(0.4f + 0.3f * lineHeight / maxHeight));
                g.drawLine(x + 1, baseY - lineHeight + 1, xEnd - 1, baseY - lineHeight + 1, 1.0f);
                
                // 상단 점 효과 (매우 높은 진폭에서만)
                if (lineHeight > maxHeight * 0.6f) {
                    float centerX = (x + xEnd) * 0.5f;
                    float dotSize = 2.0f;
                    g.setColour(dividerColour.brighter(1.0f).withAlpha(0.8f));
                    g.fillEllipse(centerX - dotSize/2, baseY - lineHeight - dotSize/2, dotSize, dotSize);
                }
            }
        }
        
        // 5. 주요 주파수 대역 구분선 (더 강조된 구분선)
        // 주요 주파수 영역 경계 계산 (기타 주요 주파수 영역)
        float lowMidBound = bounds.getX() + bounds.getWidth() * 0.2f;  // 300Hz 지점
        float midHighBound = bounds.getX() + bounds.getWidth() * 0.6f; // 1200Hz 지점
        
        // 주요 구분선 그리기
        g.setColour(juce::Colours::white.withAlpha(0.2f)); // 더 뚜렷한 색상
        g.drawLine(lowMidBound, baseY, lowMidBound, bounds.getY(), 0.7f); // 더 두꺼운 선
        g.drawLine(midHighBound, baseY, midHighBound, bounds.getY(), 0.7f);
    }
    
    // 주파수 영역 배경 표시
    void drawFrequencyRangeBackground(juce::Graphics& g, juce::Rectangle<float> bounds, float baseY)
    {
        // 주파수 영역 경계 계산 (기타 주요 주파수 영역)
        float lowMidBound = bounds.getX() + bounds.getWidth() * 0.2f;  // 300Hz 지점
        float midHighBound = bounds.getX() + bounds.getWidth() * 0.6f; // 1200Hz 지점
        
        // 매우 투명한 배경 영역 표시
        g.setColour(juce::Colours::blue.withAlpha(0.04f));       // 저주파 영역
        g.fillRect(bounds.getX(), bounds.getY(), lowMidBound - bounds.getX(), baseY - bounds.getY());
        
        g.setColour(juce::Colours::green.withAlpha(0.05f));      // 중주파 영역 (기타 핵심)
        g.fillRect(lowMidBound, bounds.getY(), midHighBound - lowMidBound, baseY - bounds.getY());
        
        g.setColour(juce::Colours::orange.withAlpha(0.04f));     // 고주파 영역
        g.fillRect(midHighBound, bounds.getY(), bounds.getRight() - midHighBound, baseY - bounds.getY());
        
        // 주파수 영역 경계선 (매우 투명)
        g.setColour(juce::Colours::white.withAlpha(0.05f));
        g.drawVerticalLine((int)lowMidBound, bounds.getY(), baseY);
        g.drawVerticalLine((int)midHighBound, bounds.getY(), baseY);
    }
    
    void updateVisualizationParams()
    {
        // 필요한 경우 크기에 따른 파라미터 조정
    }
    
    // 오디오 데이터
    juce::AudioBuffer<float> waveformData;
    juce::Array<float> frequencyData;
    std::vector<float> smoothedFrequencyData;
    std::vector<float> interpolatedData; // 보간된 주파수 데이터 (더 연속적인 시각화용)
    
    // FFT 관련 데이터
    std::unique_ptr<juce::dsp::FFT> fftEngine;
    int fftSize;
    std::vector<float> fftData;
    std::vector<float> fftDataFifo;
    int fifoIndex;
    int numBands;
    bool useLogFrequencyScale = true;
    
    // 계산 결과 캐싱 (성능 최적화)
    std::vector<std::pair<int, int>> bandIndexCache;
    
    // 이동 평균 관련 데이터
    static constexpr int movingAverageSize = 6;
    juce::Array<juce::Array<float>> frequencyHistory;
    
    // 스레드 안전성
    juce::CriticalSection audioDataLock;
    bool audioDataChanged = false;
    
    // 시간과 프레임 관련 데이터
    double lastPaintTime = 0.0;
    double lastFftTimeMs = 0.0;
    int idleFrameCount = 0;
    
    // 애니메이션 상태
    float animationOffset = 0.0f;
    
    // 색상 팔레트
    juce::Array<juce::Colour> colours;
    
    // 적응형 스케일링 및 감쇠 요소
    float dynamicScaleFactor = 270.0f;
    float globalDecayFactor = 0.0f;
    
    // 주파수 범위 관련 데이터
    float minFrequency = 80.0f;
    float maxFrequency = 2000.0f;
    
    // 그래디언트 관련 데이터
    juce::Array<juce::Colour> gradientColours;
    
    // 보간 관련 데이터
    bool useInterpolation = true;
    bool useGradientRendering = true;
    
    // 시각화 모드 설정
    VisualisationMode visualisationMode;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapleHorizontalAudioVisualiserComponent)
}; 