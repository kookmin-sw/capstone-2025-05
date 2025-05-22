#pragma once
#include <JuceHeader.h>

/**
 * 오디오 데이터를 시각적으로 표현하는 원형 시각화 컴포넌트
 * 중앙에 원이 있고 테두리에서 주파수 바가 나오는 형태로 표시합니다.
 */
class Maple3DAudioVisualiserComponent : public juce::Component,
                                public juce::Timer
{
public:
    Maple3DAudioVisualiserComponent()
    {
        // 색상 팔레트 초기화 (보라색, 핑크색, 파란색 그라데이션)
        colours.add(juce::Colour(0xffBF00FF)); // 밝은 보라색
        colours.add(juce::Colour(0xffFF00FF)); // 핑크색
        colours.add(juce::Colour(0xff8A2BE2)); // 보라색
        colours.add(juce::Colour(0xff4B0082)); // 남색
        colours.add(juce::Colour(0xff0000FF)); // 파란색
        
        // FFT 초기화
        fftSize = 1024;
        fftEngine = std::make_unique<juce::dsp::FFT>(std::log2(fftSize));
        fftData.resize(fftSize * 2, 0.0f);
        fifoIndex = 0;
        fftDataFifo.resize(fftSize, 0.0f);
        
        // 밴드 최적화 - 밴드 수 조정 (성능과 시각적 효과 균형)
        numBands = 120; // 180에서 120으로 줄여 성능 향상
        
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
        
        // 삼각 함수 캐싱 (그리기 최적화)
        precomputeTrigCache();
        
        // 타이머 설정 - 60fps 유지를 위해 16ms 간격으로 설정 (안정성을 위해 약간 여유)
        startTimer(16);
        
        // 프레임 카운터 및 속도 측정 초기화
        frameCounter = 0;
        
        // lastPaintTime 초기화
        lastPaintTime = juce::Time::getMillisecondCounterHiRes();
        lastTimerCallTime = lastPaintTime;
        
        // OpenGL 설정 - 하드웨어 가속 사용 가능하면 활성화
        setOpaque(true);
    }
    
    ~Maple3DAudioVisualiserComponent() override
    {
        // 타이머 중지
        stopTimer();
        
        // 오디오 데이터 락 획득 후 데이터 초기화
        const juce::ScopedLock sl(audioDataLock);
        
        // 데이터 구조 안전하게 정리
        waveformData.clear();
        frequencyData.clearQuick();
        smoothedFrequencyData.clear();
        fftData.clear();
        fftDataFifo.clear();
        
        // 모든 변수를 안전한 초기 상태로 리셋
        audioDataChanged = false;
        fifoIndex = 0;
        isFadingOut = false;
    }
    
    void paint(juce::Graphics& g) override
    {
        // 현재 시간 가져오기 (프레임 간 시간 계산용)
        double currentTime = juce::Time::getMillisecondCounterHiRes();
        double deltaTime = currentTime - lastPaintTime;
        lastPaintTime = currentTime;
        
        // 프레임 시간 측정 및 기록 (최대 60개 프레임 유지)
        if (frameTimeHistory.size() >= 60)
            frameTimeHistory.remove(0);
        frameTimeHistory.add((float)deltaTime);
        
        // 애니메이션 속도를 일정하게 유지 (프레임률 독립적)
        // 최적화: 애니메이션 속도 제한 - 너무 빠른 변화 방지
        float speedFactor = juce::jlimit(0.5f, 2.0f, (float)(deltaTime / 16.67));
        animationOffset += 0.04f * speedFactor;
        
        auto bounds = getLocalBounds().toFloat();
        
        // 배경 그리기 (검은색)
        g.fillAll(juce::Colour(0xff000000));
        
        // 시각화 그리기
        drawVisualization(g, bounds);
    }
    
    void resized() override
    {
        // 크기가 변경될 때 시각화 파라미터 업데이트
        updateVisualizationParams();
        
        // 크기가 변경될 때 삼각함수 캐시 다시 계산
        precomputeTrigCache();
        
        // 화면 크기에 따라 최적화된 설정
        const int width = getWidth();
        if (width < 300)
        {
            // 작은 화면에서는 더 적은 밴드 사용
            numBands = 80;
            // 데이터 초기화
            updateDataStructures();
        }
        else if (width > 600)
        {
            // 큰 화면에서는 더 많은 밴드 사용
            numBands = 120;
            // 데이터 초기화
            updateDataStructures();
        }
    }
    
    void timerCallback() override
    {
        // 타이머가 중지된 경우(소멸자에서 stopTimer 호출) 즉시 리턴
        if (!isTimerRunning())
            return;
            
        // 현재 시간 측정 - 프레임 간격 확인
        const double currentTime = juce::Time::getMillisecondCounterHiRes();
        const double elapsedMs = currentTime - lastTimerCallTime;
        lastTimerCallTime = currentTime;
        
        // 프레임 카운터 증가 (프레임 스킵 로직용)
        frameCounter++;
        
        // 프레임 속도 제한: 목표는 60fps (16.67ms)
        // 너무 빨리 호출된 경우 스킵 (CPU 사용량 감소)
        if (elapsedMs < 15.0) // 약간의 마진을 줌
            return;
            
        // 페이드 아웃 처리 - 항상 실행하되 최적화
        if (isFadingOut)
        {
            // 페이드 아웃 진행 - 더 빠른 속도로 페이드 아웃
            fadeOutFactor -= 0.15f; // 페이드 속도 증가 (더 빠르게)
            
            // 페이드 아웃이 완료되면 모든 데이터 초기화
            if (fadeOutFactor <= 0.0f)
            {
                fadeOutFactor = 0.0f;
                isFadingOut = false;
                
                // 실제 데이터 초기화
                resetData();
                
                DBG("Maple3DAudioVisualiserComponent: Fade out complete, data reset");
            }
            
            // 다시 그리기 요청
            repaint();
            return; // 페이드 아웃 중일 때는 다른 처리 하지 않음
        }
        
        // 성능 최적화: 데이터 변경이 있거나 3프레임마다 repaint 호출
        // 이는 빠른 변화가 있을 때는 고품질로, 변화가 적을 때는 자원 절약
        bool shouldRepaint = audioDataChanged || (frameCounter % 3 == 0);
        
        // audioDataChanged가 true일 때만 FFT 계산
        if (audioDataChanged)
        {
            // 새 데이터가 들어왔으므로 페이드 아웃 중단
            if (isFadingOut)
            {
                isFadingOut = false;
                fadeOutFactor = 1.0f;
            }
            
            // GUI 스레드에서 FFT 계산 (락 필요)
            const juce::ScopedLock sl(audioDataLock);
            
            // 충분한 데이터가 있는지 확인
            if (fifoIndex >= fftSize / 2)
            {
                calculateFFT();
            }
            
            audioDataChanged = false;
            shouldRepaint = true;
        }
        
        // 필요할 때만 repaint 호출하여 성능 최적화
        if (shouldRepaint)
        {
            repaint();
        }
        else
        {
            // 애니메이션 진행은 계속하되 다시 그리기는 하지 않음
            if (animationOffset > 1000.0f)
                animationOffset = 0.0f;
        }
    }
    
    // 프레임 속도 측정 및 디버깅용 메서드
    float getCurrentFrameRate() const
    {
        if (frameTimeHistory.size() < 2)
            return 0.0f;
            
        float totalTime = 0.0f;
        for (auto time : frameTimeHistory)
            totalTime += time;
            
        return 1000.0f * (frameTimeHistory.size() - 1) / totalTime;
    }
    
    // 오디오 데이터 설정 - 원본 오디오 훼손 방지 및 최적화
    void pushBuffer(const juce::AudioBuffer<float>& buffer)
    {
        // 타이머가 중지되었거나 NULL 버퍼인 경우 무시
        if (!isTimerRunning() || buffer.getNumSamples() == 0 || buffer.getNumChannels() == 0)
            return;
            
        const juce::ScopedLock sl(audioDataLock);
        
        try {
            // 버퍼 정보 가져오기
            auto numChannels = juce::jmin(buffer.getNumChannels(), 2);
            auto numSamples = buffer.getNumSamples();
            
            // 파형 표시용 데이터 저장 - 버퍼 크기가 변경된 경우에만 리사이즈
            if (waveformData.getNumChannels() != numChannels || 
                waveformData.getNumSamples() != numSamples)
            {
                waveformData.setSize(numChannels, numSamples, false, false, true);
            }
            
            // 데이터 복사 (소유권 이전)
            for (int ch = 0; ch < numChannels; ++ch)
            {
                waveformData.copyFrom(ch, 0, buffer, ch, 0, numSamples);
            }
            
            // FFT 계산을 위해 메인 채널 데이터 FIFO에 저장
            const float* samples = buffer.getReadPointer(0);
            
            // 다운샘플링 - 성능 최적화 및 배속 방지
            int stride = numSamples > fftSize ? numSamples / fftSize : 1;
            
            // FIFO 인덱스 초기화
            fifoIndex = 0;
            
            // 다운샘플링된 데이터만 처리
            for (int i = 0; i < numSamples && fifoIndex < fftSize && fifoIndex < fftDataFifo.size(); i += stride)
            {
                fftDataFifo[fifoIndex++] = samples[i];
            }
            
            // 남은 부분을 0으로 채움
            while (fifoIndex < fftSize && fifoIndex < fftDataFifo.size())
                fftDataFifo[fifoIndex++] = 0.0f;
            
            // 데이터 변경 플래그 설정 - 다음 타이머 콜백에서 FFT 계산
            audioDataChanged = true;
        }
        catch (...) {
            // 예외 발생 시 안전하게 상태 복구
            fifoIndex = 0;
            audioDataChanged = false;
        }
    }
    
    // 시각화 데이터 초기화 (재생 중지 시 호출)
    void clear()
    {
        const juce::ScopedLock sl(audioDataLock);
        
        // 페이드 아웃 효과 시작
        startFadeOut();
        
        // 타이머가 중지되었다면 즉시 데이터 초기화
        if (!isTimerRunning())
        {
            resetData();
            DBG("Maple3DAudioVisualiserComponent: Timer not running, immediate reset");
        }
        
        // 즉시 다시 그리기 요청
        repaint();
    }
    
    // 페이드 아웃 효과 시작
    void startFadeOut()
    {
        isFadingOut = true;
        fadeOutFactor = 1.0f; // 페이드 시작
    }
    
    // 실제 데이터 초기화 (페이드 아웃 후 호출)
    void resetData()
    {
        // FFT와 파형 데이터 초기화
        for (int i = 0; i < fftSize * 2; ++i)
            fftData[i] = 0.0f;
            
        for (int i = 0; i < fftSize; ++i)
            fftDataFifo[i] = 0.0f;
            
        // 파형 데이터 초기화
        waveformData.clear();
        
        // 주파수 데이터 초기화
        for (int i = 0; i < numBands; ++i)
            frequencyData.set(i, 0.0f);
            
        // 스무스 데이터 초기화
        for (int i = 0; i < numBands; ++i)
            smoothedFrequencyData[i] = 0.0f;
            
        // 이동 평균 히스토리 초기화
        for (int i = 0; i < movingAverageSize; ++i) {
            for (int j = 0; j < numBands; ++j) {
                frequencyHistory.getReference(i).set(j, 0.0f);
            }
        }
        
        fifoIndex = 0;
        
        // 상태 업데이트
        audioDataChanged = true;
    }
    
    // 주파수 스펙트럼 데이터 설정 (외부에서 계산된 경우)
    void pushFrequencyData(const juce::Array<float>& freqData)
    {
        const juce::ScopedLock sl(audioDataLock);
        
        frequencyData.clearQuick();
        frequencyData.addArray(freqData);
        
        audioDataChanged = true;
    }
    
    // 타이머 중지 시 안전한 처리를 위한 확장 메서드
    void stopTimer()
    {
        // 중지 전에 페이드 아웃 중이라면 즉시 데이터 초기화
        if (isFadingOut)
        {
            const juce::ScopedLock sl(audioDataLock);
            fadeOutFactor = 0.0f;
            isFadingOut = false;
            resetData();
            DBG("Maple3DAudioVisualiserComponent: Forced reset on timer stop");
        }
        
        // 타이머 중지
        juce::Timer::stopTimer();
    }
    
    // 컴포넌트가 소멸되기 전에 안전하게 정리
    void safeShutdown()
    {
        // 타이머 중지 및 플래그 초기화
        stopTimer();
        
        // 모든 데이터 즉시 초기화
        const juce::ScopedLock sl(audioDataLock);
        fadeOutFactor = 0.0f;
        isFadingOut = false;
        audioDataChanged = false;
        
        // 강제 초기화
        for (int i = 0; i < fftSize * 2; ++i)
            fftData[i] = 0.0f;
            
        for (int i = 0; i < fftSize; ++i)
            fftDataFifo[i] = 0.0f;
            
        // 빈 파형으로 교체 (메모리 해제)
        waveformData = juce::AudioBuffer<float>();
        
        // 주파수 데이터 초기화
        frequencyData.clear();
        
        DBG("Maple3DAudioVisualiserComponent: Safe shutdown complete");
    }
    
private:
    // 데이터 구조 업데이트 (밴드 수 변경 시 호출)
    void updateDataStructures()
    {
        // 삼각 함수 캐싱 갱신
        precomputeTrigCache();
        
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
    
    // 삼각함수 미리 계산 (성능 최적화)
    void precomputeTrigCache()
    {
        const float barWidth = juce::MathConstants<float>::twoPi / numBands;
        cosCache.resize(numBands);
        sinCache.resize(numBands);
        
        for (int i = 0; i < numBands; ++i)
        {
            float angle = i * barWidth;
            cosCache[i] = std::cos(angle);
            sinCache[i] = std::sin(angle);
        }
    }
    
    // FFT 밴드 인덱스 미리 계산 (성능 최적화)
    void precomputeBandIndices()
    {
        for (int band = 0; band < numBands; ++band)
        {
            // 로그 스케일로 밴드 인덱스 계산
            float startFraction, endFraction;
            
            if (useLogFrequencyScale)
            {
                // 로그 스케일 (낮은 주파수에 더 많은 밴드 할당)
                float logStart = std::log(1.0f) + band * (std::log(fftSize / 2.0f) - std::log(1.0f)) / numBands;
                float logEnd = std::log(1.0f) + (band + 1) * (std::log(fftSize / 2.0f) - std::log(1.0f)) / numBands;
                
                startFraction = (std::exp(logStart) - 1.0f) / (fftSize / 2.0f);
                endFraction = (std::exp(logEnd) - 1.0f) / (fftSize / 2.0f);
            }
            else
            {
                // 선형 스케일
                startFraction = band / (float)numBands;
                endFraction = (band + 1) / (float)numBands;
            }
            
            int startIndex = juce::jlimit(0, fftSize / 2, (int)(startFraction * fftSize / 2));
            int endIndex = juce::jlimit(0, fftSize / 2, (int)(endFraction * fftSize / 2));
            
            if (endIndex <= startIndex)
                endIndex = startIndex + 1;
                
            // 인덱스를 캐시에 저장
            bandIndexCache[band] = std::pair<int, int>(startIndex, endIndex);
        }
    }
    
    // FFT 계산 최적화
    void calculateFFT()
    {
        // 데이터 범위 확인
        if (fftData.empty() || fftDataFifo.empty() || frequencyData.isEmpty())
            return;
            
        // 한 번에 너무 자주 FFT를 계산하지 않도록 제한 (성능 최적화)
        double currentTimeMs = juce::Time::getMillisecondCounterHiRes();
        if (currentTimeMs - lastFftTimeMs < 25.0) // 초당 40회로 제한 (60fps보다 적게)
            return;
            
        lastFftTimeMs = currentTimeMs;
        
        try {
            // 윈도잉 함수 캐싱 - 한 번만 계산하고 재사용
            static std::vector<float> windowCoeffs;
            if (windowCoeffs.empty() || windowCoeffs.size() != fftSize)
            {
                windowCoeffs.resize(fftSize);
                for (int i = 0; i < fftSize; ++i)
                    windowCoeffs[i] = 0.5f - 0.5f * std::cos(2.0f * juce::MathConstants<float>::pi * i / (fftSize - 1));
            }
            
            // 창함수 적용 및 FFT 데이터 준비 - SIMD 최적화 가능한 형태로 변환
            for (int i = 0; i < fftSize && i * 2 + 1 < fftData.size() && i < fftDataFifo.size(); ++i)
            {
                // 캐시된 윈도우 계수 사용
                fftData[i * 2] = fftDataFifo[i] * windowCoeffs[i];
                fftData[i * 2 + 1] = 0.0f;
            }
            
            // FFT 수행
            fftEngine->performFrequencyOnlyForwardTransform(fftData.data());
            
            // 임시 주파수 데이터 저장 - 매번 새로 할당하지 않고 재사용
            // 정적 배열 사용하여 반복적인 메모리 할당 방지
            static juce::Array<float> newFrequencyData;
            const int freqDataSize = frequencyData.size();
            
            if (newFrequencyData.size() != freqDataSize)
                newFrequencyData.resize(freqDataSize);
            
            // 평균 진폭과 최대 진폭 계산 (동적 정규화용)
            float maxAmplitude = 0.0001f; // 0으로 나누기 방지
            float totalAmplitude = 0.0f;
            
            // 첫 번째 스캔: 모든 밴드의 진폭 계산하여 최대/평균값 찾기
            const int bandCacheSize = bandIndexCache.size();
            const int halfFftSize = fftSize / 2;
            
            for (int band = 0; band < numBands && band < bandCacheSize; ++band)
            {
                const auto& range = bandIndexCache[band];
                int startIndex = range.first;
                int endIndex = range.second;
                
                // 잘못된 범위 검증
                if (startIndex >= endIndex || startIndex < 0 || endIndex > halfFftSize || band >= newFrequencyData.size())
                {
                    if (band < newFrequencyData.size())
                        newFrequencyData.set(band, 0.0f);
                    continue;
                }
                    
                // 해당 밴드의 평균 스펙트럼 파워 계산
                float sum = 0.0f;
                int count = endIndex - startIndex;
                
                // 고정 범위 스캔으로 조건 검사 최적화
                for (int i = startIndex; i < endIndex; ++i)
                {
                    sum += fftData[i];
                }
                
                // 밴드의 평균 진폭 계산
                float value = count > 0 ? sum / count : 0.0f;
                
                // 최대 및 전체 진폭 업데이트
                maxAmplitude = juce::jmax(maxAmplitude, value);
                totalAmplitude += value;
                
                // 임시 저장
                newFrequencyData.set(band, value);
            }
            
            // 신호 강도 감지 (전체 진폭이 매우 낮으면 신호가 없는 것으로 간주)
            bool hasSignal = totalAmplitude > 0.01f;
            float avgAmplitude = totalAmplitude / numBands;
            
            // 전체 진폭에 따라 동적 스케일 팩터 조정 (낮은 신호에서도 좋은 시각화)
            float scaleFactor = dynamicScaleFactor;
            if (hasSignal && maxAmplitude < 0.1f)
                scaleFactor *= 1.5f;
            
            // 고정 임계값으로 연산 최적화
            const float noiseThreshold = 0.03f;
            
            // 두 번째 스캔: 동적 정규화 적용
            const float lowRangeLimit = 0.1f;
            const float highRangeStart = 0.9f;
            const float midRangeLow = 0.3f;
            const float midRangeHigh = 0.7f;
            
            for (int band = 0; band < numBands && band < newFrequencyData.size(); ++band)
            {
                float value = newFrequencyData[band];
                
                // 동적 정규화: 평균과 최대값을 고려하여 정규화
                if (hasSignal && maxAmplitude > 0.0f)
                {
                    // 로그 스케일 적용 (큰 값과 작은 값의 차이 감소)
                    // 모든 상수항 사전 계산
                    const float logScaledMax = std::log10(1.0f + maxAmplitude * scaleFactor);
                    if (logScaledMax > 0.0f)
                    {
                        float logValue = std::log10(1.0f + value * scaleFactor) / logScaledMax;
                        
                        // 특정 주파수 대역이 다른 대역보다 지나치게 강조되지 않도록 제한
                        float normalizedValue = juce::jlimit(0.0f, 0.98f, logValue);
                        
                        // 각 값이 평균보다 너무 높지 않도록 조정
                        if (avgAmplitude > 0.0001f)
                        {
                            float avgRatio = value / avgAmplitude;
                            if (avgRatio > 4.0f)
                                normalizedValue *= (4.0f / avgRatio);
                        }
                        
                        value = normalizedValue;
                    }
                    else
                    {
                        value = 0.0f;
                    }
                }
                else
                {
                    // 신호가 없으면 0으로 설정
                    value = 0.0f;
                }
                
                // 노이즈 제거 (낮은 값 필터링)
                if (value < noiseThreshold)
                    value = 0.0f;
                
                // 주파수 대역에 따른 가중치 적용 - 평탄화 (중간 주파수 강조)
                float weight = 1.0f;
                float normalizedBand = band / (float)numBands;
                
                if (normalizedBand < lowRangeLimit)  // 저주파 (더 작은 범위)
                    weight = 0.6f + normalizedBand / lowRangeLimit * 0.4f;
                else if (normalizedBand > highRangeStart)  // 고주파 (더 작은 범위)
                    weight = 0.6f + (1.0f - normalizedBand) / (1.0f - highRangeStart) * 0.4f;
                else if (normalizedBand > midRangeLow && normalizedBand < midRangeHigh)
                    weight = 1.15f;
                    
                value *= weight;
                
                newFrequencyData.set(band, value);
            }
            
            // 주파수 데이터 스무딩
            updateSmoothedFrequency(newFrequencyData);
        }
        catch (const std::exception& e) {
            // 예외 발생 시 조용히 처리
            jassertfalse; // 디버그 빌드에서만 알림
        }
    }
    
    // 주파수 데이터 스무딩 처리 최적화
    void updateSmoothedFrequency(const juce::Array<float>& newData)
    {
        // 데이터 범위 확인
        if (newData.isEmpty() || smoothedFrequencyData.empty() || frequencyHistory.isEmpty())
            return;
            
        // 히스토리에 새 데이터 추가 - 인덱스 기반 최적화
        static int historyIndex = 0;
        frequencyHistory.set(historyIndex, newData);
        historyIndex = (historyIndex + 1) % movingAverageSize;
        
        // 시간이 지나면 자동으로 모든 바가 감소하도록 전역 감쇠 적용
        globalDecayFactor = juce::jmin(1.0f, globalDecayFactor + 0.001f);
        
        // 활성 주파수가 있는지 확인
        bool hasActiveFrequency = false;
        const int newDataSize = newData.size();
        const float activeThreshold = 0.08f;
        
        for (int band = 0; band < juce::jmin(numBands, newDataSize); ++band)
        {
            if (newData[band] > activeThreshold)
            {
                hasActiveFrequency = true;
                break;
            }
        }
        
        // 활성 주파수가 있으면 글로벌 감쇠 요소 재설정
        if (hasActiveFrequency)
            globalDecayFactor = 0.0f;
        
        // 상수 캐싱으로 최적화
        const float minThreshold = 0.003f;
        const float riseSmoothing = 0.8f;
        const float fallSmoothing = 0.25f;
        const float decayMultiplier = 0.8f;
        
        for (int band = 0; band < numBands && band < smoothedFrequencyData.size(); ++band)
        {
            // 성능 최적화: 현재 값이 0이고 새로운 값도 아주 작으면 계산 건너뛰기
            if (smoothedFrequencyData[band] < minThreshold && 
                (band >= newDataSize || newData[band] < minThreshold))
            {
                smoothedFrequencyData[band] = 0.0f;
                continue;
            }
            
            float weightedSum = 0.0f;
            float totalWeight = 0.0f;
            
            // 최적화: movingAverageSize가 작으면 전체 루프 풀기
            if (movingAverageSize == 3) // 3으로 변경된 경우
            {
                // 고정 가중치 배열
                const float weights[3] = { 0.2f, 0.5f, 1.5f }; // 최신 데이터에 더 높은 가중치
                
                // 현재 인덱스에서 3개의 데이터 가져오기 (가장 오래된 것부터)
                // 인덱스 계산 최적화
                for (int i = 0; i < 3; ++i)
                {
                    int idx = (historyIndex - 3 + i + movingAverageSize) % movingAverageSize;
                    const auto& history = frequencyHistory.getReference(idx);
                    
                    if (band < history.size())
                    {
                        weightedSum += history[band] * weights[i];
                        totalWeight += weights[i];
                    }
                }
            }
            else
            {
                // 일반적인 루프 처리 - 인덱스 기반 역순 처리
                for (int i = 0; i < movingAverageSize; ++i)
                {
                    // 역순으로 접근 (최신 데이터가 더 높은 가중치를 가지도록)
                    int idx = (historyIndex - movingAverageSize + i + movingAverageSize) % movingAverageSize;
                    float weight = 0.3f + (0.7f * i / (movingAverageSize - 1));
                    
                    const auto& history = frequencyHistory.getReference(idx);
                    if (band < history.size())
                    {
                        weightedSum += history[band] * weight;
                        totalWeight += weight;
                    }
                }
            }
            
            float avg = (totalWeight > 0.0f) ? (weightedSum / totalWeight) : 0.0f;
            
            // 이전 값과 현재 값 사이의 부드러운 전이
            // 새 값이 더 높을 때는 빠르게 상승, 낮을 때는 천천히 감소
            float smoothingFactor = avg > smoothedFrequencyData[band] ? riseSmoothing : fallSmoothing;
            
            // 활성 주파수에만 반응하기 위한 디케이 효과 추가
            if (avg < minThreshold && smoothedFrequencyData[band] > 0.0f)
            {
                // 더 빠른 감소 적용 - 디케이 효과 강화
                smoothedFrequencyData[band] *= decayMultiplier;
                if (smoothedFrequencyData[band] < minThreshold)
                    smoothedFrequencyData[band] = 0.0f;
            }
            else
            {
                // 정상적인 스무딩 - 가속도 기반 스무딩 최적화
                float currentValue = smoothedFrequencyData[band];
                float targetValue = avg;
                smoothedFrequencyData[band] = currentValue + (targetValue - currentValue) * smoothingFactor;
                
                // 전역 감쇠 적용 (자동 감소) - 더 강한 감쇠
                if (globalDecayFactor > 0.05f)
                    smoothedFrequencyData[band] *= (1.02f - globalDecayFactor);
            }
            
            // 주파수 값에 한계 설정 (특정 주파수가 지나치게 강조되는 것 방지)
            smoothedFrequencyData[band] = juce::jmin(0.98f, smoothedFrequencyData[band]);
        }
        
        // 주파수 데이터가 변경되었음을 표시
        audioDataChanged = true;
    }
    
    // 시각화 그리기 최적화
    void drawVisualization(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const juce::ScopedLock sl(audioDataLock);
        
        auto centerX = bounds.getCentreX();
        auto centerY = bounds.getCentreY();
        
        // 중앙에 어두운 원 그리기
        float innerRadius = bounds.getWidth() * 0.16f;
        drawInnerCircle(g, centerX, centerY, innerRadius);
        
        // 주파수 바 그리기 (최적화된 버전)
        drawFrequencyBars(g, centerX, centerY, innerRadius);
    }
    
    // 주파수 바 그리기 최적화
    void drawFrequencyBars(juce::Graphics& g, float centerX, float centerY, float innerRadius)
    {
        if (smoothedFrequencyData.empty())
            return;
            
        // 각 바의 너비는 고정값 - 매번 계산할 필요 없음
        const float barWidth = juce::MathConstants<float>::twoPi / numBands;
        const float animAngleOffset = animationOffset * 0.05f;
        
        // 최대 바 길이
        const float maxBarLength = innerRadius * 0.65f;
        
        // 색상 인덱스 계산은 한 번만 수행
        const int colorOffset = (int)(animationOffset * 0.01f);
        
        // 페이드 아웃 중인 경우 추가 연산 방지
        const float fadeFactor = isFadingOut ? fadeOutFactor : 1.0f;
        
        // 성능 최적화: 그릴 바 개수 미리 계산 - 활성 밴드 필터링
        // 이 배열을 재사용하면 반복적인 메모리 할당 방지
        static std::vector<int> activeBands;
        activeBands.clear();
        
        // 먼저 활성 밴드만 필터링하여 그리기 작업 최소화
        for (int i = 0; i < numBands && i < smoothedFrequencyData.size(); ++i)
        {
            if (smoothedFrequencyData[i] >= 0.003f)
                activeBands.push_back(i);
        }
        
        // 주파수별 바 그리기 (활성화된 밴드만)
        for (int idx = 0; idx < activeBands.size(); ++idx)
        {
            int i = activeBands[idx];
            
            // 바의 각도 계산 (애니메이션 각도 오프셋 적용)
            float angle = i * barWidth + animAngleOffset;
            
            // 삼각 함수 직접 계산 (애니메이션 오프셋 때문에 캐시 사용 불가)
            float cosAngle = std::cos(angle);
            float sinAngle = std::sin(angle);
            
            // 바의 길이 계산 (주파수 값에 따라) - 비선형 매핑 적용
            float value = smoothedFrequencyData[i];
            
            // 비선형 변환으로 낮은 값은 더 낮게, 높은 값은 더 높게
            // 페이드 아웃 중이면 페이드 효과 적용
            float barLength = maxBarLength * std::pow(value, 1.2f) * fadeFactor;
            
            // 최소 길이 제한으로 매우 작은 바는 그리지 않음 - 성능 최적화
            if (barLength < 1.0f)
                continue;
            
            // 안쪽과 바깥쪽 좌표 계산
            float innerX = centerX + cosAngle * innerRadius;
            float innerY = centerY + sinAngle * innerRadius;
            float outerX = centerX + cosAngle * (innerRadius + barLength);
            float outerY = centerY + sinAngle * (innerRadius + barLength);
            
            // 날카로운 끝을 위한 중간 지점 계산
            float midPointRatio = 0.65f;
            float midX = innerX + (outerX - innerX) * midPointRatio;
            float midY = innerY + (outerY - innerY) * midPointRatio;
            
            // 색상 선택 (색상 인덱스 회전) - 최적화된 계산
            int colorIndex = ((i * colours.size() / numBands) + colorOffset) % colours.size();
            juce::Colour barColour = colours[colorIndex];
            
            // 색상 alpha/밝기 계산 (주파수 값과 위치에 따라)
            float alpha = juce::jlimit(0.4f, 1.0f, value * 1.8f);
            alpha *= fadeFactor; // 페이드 아웃 적용
                
            float brightness = juce::jlimit(0.9f, 2.0f, 1.0f + value * 1.2f);
            
            // 바 각도 위치에 따라 두께 조정 (더 연속적인 느낌)
            if (i % 2 == 0)
                brightness *= 1.15f; // 짝수 바는 더 밝게
            
            // 바의 두께 계산 (주파수 값에 따라 비선형으로 조정)
            float baseThickness = 2.5f + value * value * 6.0f;
            float innerThickness = baseThickness;
            float midThickness = baseThickness * 0.65f;
            float outerThickness = baseThickness * 0.25f;
            
            // 바 각도 위치에 따라 두께 약간 조정
            if (i % 2 == 0)
                innerThickness *= 1.15f;
                
            // 1. 내부영역 - 기본 바 그리기
            juce::Colour innerColour = barColour.withAlpha(alpha * 0.5f);
            g.setColour(innerColour);
            g.drawLine(innerX, innerY, midX, midY, innerThickness);
            
            // 2. 중간영역 - 빛나는 효과 추가
            juce::ColourGradient gradient;
            gradient.point1 = juce::Point<float>(innerX, innerY);
            gradient.point2 = juce::Point<float>(outerX, outerY);
            gradient.isRadial = false;
            
            // 그라데이션 정의 (중심에서 바깥쪽으로) - 최적화: 색상 수 제한
            gradient.addColour(0.0, barColour.withAlpha(alpha * 0.6f).withMultipliedBrightness(1.3f));
            gradient.addColour(0.7, barColour.withAlpha(alpha * 0.8f).withMultipliedBrightness(brightness));
            gradient.addColour(1.0, barColour.withAlpha(alpha * 0.3f).withMultipliedBrightness(brightness * 0.7f));
            
            g.setGradientFill(gradient);
            g.drawLine(midX - cosAngle * 0.5f, midY - sinAngle * 0.5f, outerX, outerY, midThickness);
        }
    }
    
    // 중앙 원 그리기 최적화
    void drawInnerCircle(juce::Graphics& g, float centerX, float centerY, float radius)
    {
        // 정적 그라데이션으로 재사용
        static juce::ColourGradient gradient;
        static float lastRadius = 0.0f;
        static float lastCenterX = 0.0f;
        static float lastCenterY = 0.0f;
        
        // 중심이나 크기가 변경된 경우에만 그라데이션 다시 계산
        if (std::abs(lastRadius - radius) > 0.1f || 
            std::abs(lastCenterX - centerX) > 0.1f || 
            std::abs(lastCenterY - centerY) > 0.1f)
        {
            lastRadius = radius;
            lastCenterX = centerX;
            lastCenterY = centerY;
            
            gradient = juce::ColourGradient();
            gradient.point1 = juce::Point<float>(centerX, centerY);
            gradient.point2 = juce::Point<float>(centerX + radius, centerY + radius);
            gradient.isRadial = true;
            
            // 그라데이션 색상 (매우 어두운 파란색에서 검은색)
            gradient.addColour(0.0, juce::Colour(0xff0A1A40));
            gradient.addColour(1.0, juce::Colour(0xff000000));
        }
        
        g.setGradientFill(gradient);
        g.fillEllipse(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f);
        
        // 얇은 테두리 그리기 - 페이드 아웃 중일 경우 알파값 조정
        float alpha = isFadingOut ? 0.5f * fadeOutFactor : 0.5f;
        g.setColour(juce::Colour(0xff1A1A40).withAlpha(alpha));
        g.drawEllipse(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f, 0.5f);
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
    std::vector<float> cosCache;
    std::vector<float> sinCache;
    
    // 이동 평균 관련 데이터
    static constexpr int movingAverageSize = 8; // 4에서 3으로 줄여 더 빠른 반응성 제공
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
    float dynamicScaleFactor = 250.0f; // 값 증가
    float globalDecayFactor = 0.0f;
    
    double lastTimerCallTime = 0.0;
    
    // 페이드 아웃 관련 상태
    bool isFadingOut = false;
    float fadeOutFactor = 1.0f;
    
    // 프레임 속도 측정용 변수 추가
    juce::Array<float> frameTimeHistory;
    int frameCounter = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Maple3DAudioVisualiserComponent)
}; 