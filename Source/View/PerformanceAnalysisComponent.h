#pragma once
#include <JuceHeader.h>
#include "LookAndFeel/MapleTheme.h"

// Performance analysis display component
class PerformanceAnalysisComponent : public juce::Component,
                                    public juce::Timer
{
public:
    PerformanceAnalysisComponent() : accuracyMeter(progressValue)
    {
        addAndMakeVisible(accuracyMeter);
        addAndMakeVisible(titleLabel);
        addAndMakeVisible(accuracyLabel);
        addAndMakeVisible(timingLabel);
        addAndMakeVisible(difficultyLabel);
        
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
        startTimer(30); // Start animation timer
    }
    
    ~PerformanceAnalysisComponent() override
    {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleTheme::getCardColour());
        
        auto bounds = getLocalBounds().reduced(10);
        
        // Heatmap area
        auto heatmapArea = bounds.removeFromBottom(bounds.getHeight() / 3);
        g.setColour(MapleTheme::getCardColour().darker(0.1f));
        g.fillRoundedRectangle(heatmapArea.toFloat().reduced(5.0f), 5.0f);
        
        // Heatmap title
        g.setColour(MapleTheme::getTextColour());
        g.setFont(15.0f);
        g.drawText("Performance Heatmap", heatmapArea.removeFromTop(25), juce::Justification::centred, true);
        
        // Score position markers
        g.setFont(12.0f);
        g.drawText("Start", heatmapArea.getX() + 10, heatmapArea.getBottom() - 20, 40, 20, juce::Justification::centredLeft);
        g.drawText("End", heatmapArea.getRight() - 50, heatmapArea.getBottom() - 20, 40, 20, juce::Justification::centredRight);
        
        // Draw demo heatmap
        g.setColour(juce::Colours::red.withAlpha(0.7f));
        auto barWidth = (heatmapArea.getWidth() - 30) / 10;
        auto barX = heatmapArea.getX() + 15;
        
        for (int i = 0; i < 10; ++i)
        {
            float height = (std::sin((i + animationOffset) * 0.5f) + 1.0f) * 30.0f + 10.0f;
            if (i == 3 || i == 7) height += 20.0f; // Emphasize difficult sections
            
            g.fillRoundedRectangle(barX, heatmapArea.getBottom() - height - 25,
                                  barWidth - 5, height, 3.0f);
            barX += barWidth;
        }
        
        // Graph area
        auto graphArea = bounds.removeFromRight(bounds.getWidth() * 0.6f);
        g.setColour(MapleTheme::getCardColour().darker(0.1f));
        g.fillRoundedRectangle(graphArea.toFloat().reduced(5.0f), 5.0f);
        
        // Graph title
        g.setColour(MapleTheme::getTextColour());
        g.setFont(15.0f);
        g.drawText("Timing Accuracy (ms)", graphArea.removeFromTop(25), juce::Justification::centred, true);
        
        // Draw demo graph
        g.setColour(juce::Colour(0xff42a5f5));
        auto zeroLine = graphArea.getCentreY();
        auto graphWidth = graphArea.getWidth() - 30;
        auto graphX = graphArea.getX() + 15;
        auto lastX = graphX;
        auto lastY = zeroLine;
        
        g.drawHorizontalLine(zeroLine, graphX, graphX + graphWidth);
        
        g.setColour(juce::Colour(0xff42a5f5).withAlpha(0.7f));
        for (int i = 0; i < graphWidth; i += 3)
        {
            float delta = std::sin((i + animationOffset * 2) * 0.05f) * 20.0f;
            auto x = graphX + i;
            auto y = zeroLine + delta;
            
            if (i > 0)
                g.drawLine(lastX, lastY, x, y, 1.5f);
            
            lastX = x;
            lastY = y;
        }
        
        // Draw scales
        g.setColour(MapleTheme::getTextColour().withAlpha(0.5f));
        g.setFont(12.0f);
        g.drawText("+50ms", graphArea.getX() + 10, zeroLine - 50, 50, 20, juce::Justification::centredLeft);
        g.drawText("-50ms", graphArea.getX() + 10, zeroLine + 30, 50, 20, juce::Justification::centredLeft);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(15);
        
        // Title
        titleLabel.setBounds(bounds.removeFromTop(30));
        
        // Exclude bottom heatmap area
        auto heatmapHeight = bounds.getHeight() / 3;
        bounds.removeFromBottom(heatmapHeight);
        
        // Exclude graph area
        bounds.removeFromRight(bounds.getWidth() * 0.6f);
        
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
    
private:
    juce::Label titleLabel;
    juce::Label accuracyLabel;
    juce::Label timingLabel;
    juce::Label difficultyLabel;
    
    double progressValue = 0.8; // Value that ProgressBar will watch
    juce::ProgressBar accuracyMeter; // ProgressBar initialized with progressValue reference in constructor
    
    float noteAccuracy = 0.8f;    // Note accuracy (0-1 range)
    float timingAccuracy = 0.7f;  // Timing accuracy (0-1 range)
    float animationOffset = 0.0f; // Animation offset
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PerformanceAnalysisComponent)
}; 