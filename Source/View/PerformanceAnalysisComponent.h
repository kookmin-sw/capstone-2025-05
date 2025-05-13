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
    
    // 오디오 데이터를 시각화 컴포넌트로 전달하는 메서드
    void pushAudioBuffer(const juce::AudioBuffer<float>& buffer)
    {
        visualiserComponent.pushBuffer(buffer);
        horizontalVisualiserComponent.pushBuffer(buffer);
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