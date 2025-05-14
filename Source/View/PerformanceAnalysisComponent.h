#pragma once
#include <JuceHeader.h>
#include "LookAndFeel/MapleTheme.h"
#include "Maple3DAudioVisualiserComponent.h"
#include "View/MapleHorizontalAudioVisualiserComponent.h"

// Performance analysis display component
class PerformanceAnalysisComponent : public juce::Component,
                                    public juce::Timer
{
public:
    // Structure to hold note accuracy data
    struct NoteAccuracy
    {
        int time;           // Time position (could be tick or note index)
        float accuracy;     // Accuracy value (0-1)
        bool isCorrect;     // Whether the note was played correctly
    };

    PerformanceAnalysisComponent() : accuracyMeter(progressValue)
    {
        addAndMakeVisible(accuracyMeter);
        addAndMakeVisible(titleLabel);
        addAndMakeVisible(accuracyLabel);
        addAndMakeVisible(timingLabel);
        addAndMakeVisible(difficultyLabel);
        addAndMakeVisible(visualiserComponent); // 3D 시각화 컴포넌트 추가
        addAndMakeVisible(horizontalVisualiserComponent); // 수평 시각화 컴포넌트 추가
        
        // 수평 시각화 컴포넌트의 진폭 감도 조절
        horizontalVisualiserComponent.setDynamicScaleFactor(150.0f); // 기본값 270.0f에서 감소
        
        // 시각화 모드 초기화 - 바 모드로 고정
        horizontalVisualiserComponent.setVisualisationMode(MapleHorizontalAudioVisualiserComponent::BarSpectrum);
        
        // 기타 주파수 범위 초기화 (기타 주파수 범위로 제한)
        // 일반적인 기타 음역 범위: 표준 튜닝 E2(82Hz)~E6(1318Hz), 하모닉스 포함 ~2kHz
        const float minFreq = 80.0f;   // 최저 주파수 (Hz) - 가장 낮은 E 음 커버
        const float maxFreq = 2000.0f; // 최고 주파수 (Hz) - 하모닉스 포함
        horizontalVisualiserComponent.setFrequencyRange(minFreq, maxFreq);
        
        titleLabel.setText("Performance Analysis", juce::dontSendNotification);
        titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
        titleLabel.setJustificationType(juce::Justification::centred);
        
        accuracyLabel.setText("Accuracy: Needs analysis", juce::dontSendNotification);
        accuracyLabel.setJustificationType(juce::Justification::centredLeft);
        
        timingLabel.setText("Timing Accuracy: Needs analysis", juce::dontSendNotification);
        timingLabel.setJustificationType(juce::Justification::centredLeft);
        
        difficultyLabel.setText("Difficult sections: Needs analysis", juce::dontSendNotification);
        difficultyLabel.setJustificationType(juce::Justification::centredLeft);
        
        // Set demo data
        setPerformanceData(0.0f, 0.0f);
        
        // Initialize note accuracy data for demo
        for (int i = 0; i < 32; ++i)
        {
            // Generate random accuracy values for demo
            float accuracy = juce::Random::getSystemRandom().nextFloat();
            bool isCorrect = accuracy > 0.3f;
            
            NoteAccuracy note;
            note.time = i;
            note.accuracy = accuracy;
            note.isCorrect = isCorrect;
            
            noteAccuracyData.add(note);
        }
        
        startTimer(30); // Start animation timer
    }
    
    ~PerformanceAnalysisComponent() override
    {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleTheme::getCardColour());
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(15);
        
        // Title
        titleLabel.setBounds(bounds.removeFromTop(30));
        
        // 수평 오디오 시각화 컴포넌트를 하단에 배치
        auto horizontalVisualiserHeight = 150;
        auto horizontalVisualiserArea = bounds.removeFromBottom(horizontalVisualiserHeight);
        
        // 수평 시각화 컴포넌트 배치 (전체 공간 사용)
        horizontalVisualiserComponent.setBounds(horizontalVisualiserArea.reduced(5));
        
        // 3D 시각화 컴포넌트를 오른쪽에 배치
        auto visualiserArea = bounds.removeFromRight(bounds.getWidth() * 0.6f);
        visualiserComponent.setBounds(visualiserArea.reduced(5));
        
        // Metric labels layout
        auto labelHeight = 25;
        accuracyMeter.setBounds(bounds.removeFromTop(40));
        bounds.removeFromTop(10); // Spacing
        accuracyLabel.setBounds(bounds.removeFromTop(labelHeight));
        bounds.removeFromTop(5); // Spacing
        timingLabel.setBounds(bounds.removeFromTop(labelHeight));
        bounds.removeFromTop(5); // Spacing
        difficultyLabel.setBounds(bounds.removeFromTop(labelHeight));
    }
    
    void timerCallback() override
    {
        // Update animation
        animationOffset += 1.0f;
        if (animationOffset > 1000.0f)
            animationOffset = 0.0f;
            
        // Refresh UI
        repaint();
    }
    
    void setPerformanceData(float noteAccuracy, float timingAccuracy)
    {
        this->noteAccuracy = noteAccuracy;
        this->timingAccuracy = timingAccuracy;
        
        // Update progress value that ProgressBar is watching
        progressValue = noteAccuracy;
        
        // In real implementation, update labels based on analysis results
        // For now, just display demo values
        accuracyLabel.setText("Accuracy: " + juce::String(int(noteAccuracy * 100.0f)) + "%", 
                             juce::dontSendNotification);
        
        timingLabel.setText("Timing Accuracy: " + juce::String(int(timingAccuracy * 100.0f)) + "%", 
                           juce::dontSendNotification);
                           
        // In real implementation this would be called after analysis
        startTimer(30);
    }
    
    // In real implementation, this would be called when note accuracy data is available
    void setNoteAccuracyData(const juce::Array<NoteAccuracy>& data)
    {
        noteAccuracyData = data;
        repaint();
    }
    
    // 오디오 데이터를 시각화 컴포넌트로 전달하는 메서드 (재생 데이터용)
    void pushAudioBuffer(const juce::AudioBuffer<float>& buffer)
    {
        DBG("pushAudioBuffer size: " + juce::String(buffer.getNumSamples()));
        visualiserComponent.pushBuffer(buffer);
    }
    
    // 마이크 입력 데이터를 수평 시각화 컴포넌트로 전달하는 메서드
    void pushMicrophoneBuffer(const juce::AudioBuffer<float>& buffer)
    {
        // 마이크 입력 데이터의 진폭을 조절하기 위한 임시 버퍼
        static juce::AudioBuffer<float> scaledBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        
        // 입력 버퍼 복사
        scaledBuffer.clear();
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            scaledBuffer.copyFrom(ch, 0, buffer, ch, 0, buffer.getNumSamples());
        }
        
        // 진폭 스케일 조절 (값을 작게 만들어 최대치까지 올라가는 것을 방지)
        // 적당한 스케일 값을 찾아 조절. 필요에 따라 이 값을 변경할 수 있음
        float scaleFactor = 0.15f; // 0.3f에서 더 감소
        
        for (int ch = 0; ch < scaledBuffer.getNumChannels(); ++ch)
        {
            auto* channelData = scaledBuffer.getWritePointer(ch);
            for (int i = 0; i < scaledBuffer.getNumSamples(); ++i)
            {
                channelData[i] *= scaleFactor;
            }
        }
        
        // 스케일이 조절된 데이터를 시각화 컴포넌트로 전달
        horizontalVisualiserComponent.pushBuffer(scaledBuffer);
    }
    
    // 시각화 컴포넌트를 초기화하는 메서드 (재생 중지 시 호출)
    void clearVisualization()
    {
        // 두 시각화 컴포넌트를 모두 초기화
        clear3DVisualization();
        clearHorizontalVisualization();
    }
    
    // 3D 시각화만 초기화하는 메서드 (재생 중지 시 호출)
    void clear3DVisualization()
    {
        // 3D 시각화 초기화
        visualiserComponent.clear();
    }
    
    // 수평 시각화만 초기화하는 메서드 (마이크 모니터링 비활성화 시 호출)
    void clearHorizontalVisualization()
    {
        // 빈 버퍼를 생성하여 수평 시각화 초기화
        juce::AudioBuffer<float> emptyBuffer(2, 1024);
        emptyBuffer.clear(); // 모든 샘플을 0으로 설정
        
        // 수평 시각화 초기화
        horizontalVisualiserComponent.clear();
    }
    
    // 안전한 종료를 위한 메서드 (GuitarPracticeComponent 소멸자에서 호출)
    void safeShutdown()
    {
        // 내부 타이머 중지
        stopTimer();
        
        // 3D 시각화 안전 종료
        visualiserComponent.safeShutdown();
        
        // 수평 시각화 초기화
        horizontalVisualiserComponent.clear();
        horizontalVisualiserComponent.stopTimer();
        
        DBG("PerformanceAnalysisComponent: Safe shutdown complete");
    }
    
    // 내부 컴포넌트 타이머 중지
    void stopTimer()
    {
        // 내부 컴포넌트의 타이머 중지
        visualiserComponent.stopTimer();
        horizontalVisualiserComponent.stopTimer();
    }

private:
    juce::Label titleLabel;
    juce::Label accuracyLabel;
    juce::Label timingLabel;
    juce::Label difficultyLabel;
    
    Maple3DAudioVisualiserComponent visualiserComponent; // 3D 시각화 컴포넌트
    MapleHorizontalAudioVisualiserComponent horizontalVisualiserComponent; // 수평 시각화 컴포넌트
    
    double progressValue = 0.8; // Value that ProgressBar will watch
    juce::ProgressBar accuracyMeter; // ProgressBar initialized with progressValue reference in constructor
    
    float noteAccuracy = 0.8f;    // Note accuracy (0-1 range)
    float timingAccuracy = 0.7f;  // Timing accuracy (0-1 range)
    float animationOffset = 0.0f; // Animation offset
    
    juce::Array<NoteAccuracy> noteAccuracyData; // Store individual note accuracy data
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PerformanceAnalysisComponent)
}; 