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
        // 색상 팔레트 초기화 - 기타 주파수에 적합한 색상으로 변경
        colours.add(juce::Colour(0xff3366FF)); // 파랑 (저주파 영역 80-300Hz)
        colours.add(juce::Colour(0xff33CCFF)); // 하늘색 (낮은 중주파 300-600Hz)
        colours.add(juce::Colour(0xff00FF99)); // 연두색 (중간 주파수 600-1000Hz - 기타 핵심 영역)
        colours.add(juce::Colour(0xffFFCC00)); // 주황색 (높은 중주파 1000-1500Hz)
        colours.add(juce::Colour(0xffFF6600)); // 오렌지색 (고주파 1500-2000Hz)
        
        // FFT 초기화
        fftSize = 1024;
        fftEngine = std::make_unique<juce::dsp::FFT>(std::log2(fftSize));
        fftData.resize(fftSize * 2, 0.0f);
        fifoIndex = 0;
        fftDataFifo.resize(fftSize, 0.0f);
        
        // 주파수 밴드 설정 - 수평 바에 최적화된 수
        numBands = 64; // 수평 시각화에 적합한 밴드 수
        
        // 스펙트럼 데이터 초기화
        for (int i = 0; i < numBands; ++i)
            frequencyData.add(0.0f);
            
        // 이동 평균 필터 히스토리 초기화
        for (int i = 0; i < movingAverageSize; ++i) {
            juce::Array<float> history;
            for (int j = 0; j < numBands; ++j) {
                history.add(0.0f);
            }
            frequencyHistory.add(history);
        }
        
        // 스무스 값 초기화
        smoothedFrequencyData.resize(numBands, 0.0f);
            
        // 파형 데이터 초기화
        waveformData.setSize(2, 1024);
        
        // 밴드별 인덱스 캐싱 (FFT 계산 최적화)
        bandIndexCache.resize(numBands);
        
        // 밴드 인덱스 미리 계산
        precomputeBandIndices();
        
        // 타이머 시작
        startTimerHz(60);
        
        // 마지막 페인트 시간 초기화
        lastPaintTime = juce::Time::getMillisecondCounterHiRes();
        
        // 불투명 설정 (렌더링 최적화)
        setOpaque(true);
        
        // 기타 주파수 특화 설정
        dynamicScaleFactor = 270.0f; // 진폭 스케일 증가
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
        // 기타 주파수 범위 설정 (80Hz ~ 2000Hz)
        const float minFrequency = 80.0f;
        const float maxFrequency = 2000.0f;
        
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
    }
    
    // 수평 주파수 바 그리기
    void drawHorizontalBars(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const juce::ScopedLock sl(audioDataLock);
        
        // 바 치수 계산
        float barWidth = bounds.getWidth() / numBands;
        float maxBarHeight = bounds.getHeight() * 0.85f; // 화면 높이의 85%를 최대 높이로 설정
        float baseY = bounds.getBottom() - 10.0f; // 약간의 여백
        
        // 바의 패딩 계산 (각 바 사이의 간격)
        float barPadding = juce::jmin(barWidth * 0.2f, 3.0f);
        
        // 배경에 주파수 영역 표시 (옵션)
        drawFrequencyRangeBackground(g, bounds, baseY);
        
        // 각 주파수 밴드에 대한 바 그리기
        for (int band = 0; band < numBands; ++band)
        {
            // 값이 0이면 건너뛰기 (성능 최적화)
            float value = band < smoothedFrequencyData.size() ? smoothedFrequencyData[band] : 0.0f;
            if (value <= 0.001f)
                continue;
                
            // 바의 높이 계산
            float barHeight = value * maxBarHeight;
            
            // 바의 위치 계산
            float x = bounds.getX() + band * barWidth;
            float y = baseY - barHeight;
            
            // 주파수 영역에 따른 색상 설정
            juce::Colour barColour;
            float normalizedBand = (float)band / numBands;
            
            // 기타 주파수 영역별 색상 맵핑
            if (normalizedBand < 0.2f) {
                // 저주파 영역 (파란색 계열)
                barColour = colours[0];
            }
            else if (normalizedBand < 0.35f) {
                // 낮은 중주파 영역 (하늘색)
                barColour = colours[1];
            }
            else if (normalizedBand < 0.6f) {
                // 중간 주파수 영역 (연두색) - 기타 핵심 영역
                barColour = colours[2];
            }
            else if (normalizedBand < 0.8f) {
                // 높은 중주파 영역 (주황색)
                barColour = colours[3];
            }
            else {
                // 고주파 영역 (오렌지색)
                barColour = colours[4];
            }
            
            // 진폭에 따른 밝기 설정 - 더 역동적인 효과
            float brightness = 0.7f + 0.5f * value;
            barColour = barColour.withMultipliedBrightness(brightness);
            
            // 애니메이션 효과를 위한 알파값 변동
            float alpha = 0.9f + 0.1f * std::sin(animationOffset * 0.1f + band * 0.2f);
            
            // 바 그리기 (약간의 둥근 모서리로)
            g.setColour(barColour.withAlpha(alpha));
            
            // 기타 주파수 영역에서는 더 두껍고 눈에 띄게 표현
            if (normalizedBand >= 0.2f && normalizedBand < 0.6f) {
                // 더 넓은 바 (패딩 줄임)
                g.fillRoundedRectangle(x + barPadding * 0.3f, y, barWidth - barPadding * 0.6f, barHeight, 2.5f);
                
                // 더 강한 상단 하이라이트
                g.setColour(barColour.brighter(0.4f).withAlpha(alpha));
                g.fillRoundedRectangle(x + barPadding * 0.3f, y, barWidth - barPadding * 0.6f, 3.5f, 1.8f);
            } 
            else {
                // 일반 바
                g.fillRoundedRectangle(x + barPadding * 0.5f, y, barWidth - barPadding, barHeight, 2.0f);
                
                // 상단 하이라이트 (3D 효과)
                g.setColour(barColour.brighter(0.3f).withAlpha(alpha));
                g.fillRoundedRectangle(x + barPadding * 0.5f, y, barWidth - barPadding, 2.5f, 1.5f);
            }
            
            // 글로우 효과 (높은 값에서만) - 활성화된 주파수 강조
            if (value > 0.5f)
            {
                float glowSize = barWidth * (0.8f + 0.2f * value); // 값이 클수록 더 큰 글로우
                float glowY = y - 2.0f;
                float glowX = x + (barWidth - glowSize) * 0.5f;
                
                // 값이 높을수록 더 밝은 글로우
                float glowBrightness = 0.5f + 0.5f * value;
                g.setColour(barColour.brighter(glowBrightness).withAlpha(0.3f + 0.1f * value));
                g.fillEllipse(glowX, glowY - glowSize * 0.3f, glowSize, glowSize * 0.6f);
            }
        }
        
        // 가이드 라인 (선택 사항)
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawHorizontalLine((int)baseY, bounds.getX(), bounds.getRight());
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapleHorizontalAudioVisualiserComponent)
}; 