#pragma once
#include <JuceHeader.h>
#include "LookAndFeel/MapleTheme.h"

// Practice progress display component
class PracticeProgressComponent : public juce::Component
{
public:
    PracticeProgressComponent()
    {
        // Initialize labels
        addAndMakeVisible(titleLabel);
        titleLabel.setText("Practice Progress", juce::dontSendNotification);
        titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
        titleLabel.setJustificationType(juce::Justification::centred);
        
        // Initialize chart buttons
        addAndMakeVisible(weekButton);
        addAndMakeVisible(monthButton);
        addAndMakeVisible(yearButton);
        
        weekButton.setButtonText("Weekly");
        monthButton.setButtonText("Monthly");
        yearButton.setButtonText("Yearly");
        
        weekButton.setRadioGroupId(1);
        monthButton.setRadioGroupId(1);
        yearButton.setRadioGroupId(1);
        
        weekButton.setClickingTogglesState(true);
        monthButton.setClickingTogglesState(true);
        yearButton.setClickingTogglesState(true);
        
        weekButton.setToggleState(true, juce::dontSendNotification);
        
        // Set demo data
        setDemoData();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleTheme::getCardColour());
        
        auto bounds = getLocalBounds().reduced(15);
        bounds.removeFromTop(70); // Exclude title and button area
        
        // Graph area
        auto chartArea = bounds.removeFromLeft(bounds.getWidth() * 0.6f);
        g.setColour(MapleTheme::getCardColour().darker(0.1f));
        g.fillRoundedRectangle(chartArea.toFloat().reduced(10.0f), 5.0f);
        
        // Chart title
        g.setColour(MapleTheme::getTextColour());
        g.setFont(15.0f);
        g.drawText("Practice Time Statistics", chartArea.removeFromTop(25).reduced(10, 0), 
                  juce::Justification::centred, true);
                  
        // Draw graph data
        drawBarChart(g, chartArea.reduced(20, 15));
        
        // Stats area
        auto statsArea = bounds;
        g.setColour(MapleTheme::getCardColour().darker(0.1f));
        g.fillRoundedRectangle(statsArea.toFloat().reduced(10.0f), 5.0f);
        
        // Stats title
        g.setColour(MapleTheme::getTextColour());
        g.setFont(15.0f);
        g.drawText("Practice Statistics", statsArea.removeFromTop(25).reduced(10, 0), 
                 juce::Justification::centred, true);
                 
        // Display statistics data
        statsArea = statsArea.reduced(15, 10);
        g.setFont(14.0f);
        
        auto drawStat = [&](const juce::String& label, const juce::String& value)
        {
            auto row = statsArea.removeFromTop(25);
            g.drawText(label, row.removeFromLeft(row.getWidth() * 0.6f), 
                      juce::Justification::centredLeft, true);
            g.drawText(value, row, juce::Justification::centredRight, true);
        };
        
        drawStat("Total Practice Time", "32h 15m");
        drawStat("This Week's Practice", "4h 45m");
        drawStat("Consecutive Practice Days", "5 days");
        drawStat("Most Practiced Song", "Canon Rock");
        drawStat("Songs Practiced", "8 songs");
        drawStat("Average Accuracy", "76%");
        drawStat("Highest Accuracy", "92%");
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(15);
        
        // Title label
        titleLabel.setBounds(bounds.removeFromTop(30));
        
        // Button layout
        auto buttonHeight = 25;
        auto buttonArea = bounds.removeFromTop(30);
        auto buttonWidth = 80;
        
        // Center-align buttons
        auto startX = (buttonArea.getWidth() - (3 * buttonWidth + 20)) / 2;
        weekButton.setBounds(buttonArea.getX() + startX, buttonArea.getY(), buttonWidth, buttonHeight);
        monthButton.setBounds(weekButton.getRight() + 10, buttonArea.getY(), buttonWidth, buttonHeight);
        yearButton.setBounds(monthButton.getRight() + 10, buttonArea.getY(), buttonWidth, buttonHeight);
    }
    
private:
    void drawBarChart(juce::Graphics& g, juce::Rectangle<int> area)
    {
        const int numBars = 7; // One week
        float barWidth = area.getWidth() / static_cast<float>(numBars);
        float maxValue = 0.0f;
        
        // Find max value
        for (auto value : weeklyData)
            maxValue = juce::jmax(maxValue, value);
            
        if (maxValue == 0.0f)
            maxValue = 1.0f; // Prevent division by zero
        
        // Draw X axis
        g.setColour(MapleTheme::getTextColour().withAlpha(0.5f));
        g.drawLine(area.getX(), area.getBottom(), area.getRight(), area.getBottom(), 1.0f);
        
        // Y axis values
        g.setFont(12.0f);
        g.drawText("0", area.getX() - 25, area.getBottom() - 10, 20, 20, juce::Justification::centredRight);
        g.drawText(juce::String(static_cast<int>(maxValue)), area.getX() - 25, area.getY() - 10, 20, 20, juce::Justification::centredRight);
        
        // Draw bars
        const juce::String dayLabels[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        
        for (int i = 0; i < numBars; ++i)
        {
            float value = weeklyData[i];
            float normalizedHeight = (value / maxValue) * area.getHeight();
            float x = area.getX() + i * barWidth;
            
            // Bar color
            g.setColour(barColors[i]);
            g.fillRoundedRectangle(x + 5, area.getBottom() - normalizedHeight, barWidth - 10, normalizedHeight, 4.0f);
            
            // X axis labels
            g.setColour(MapleTheme::getTextColour());
            g.setFont(12.0f);
            g.drawText(dayLabels[i], x, area.getBottom() + 5, barWidth, 15, juce::Justification::centred);
        }
    }
    
    void setDemoData()
    {
        // Weekly practice time (hours)
        weeklyData = {1.5f, 0.5f, 2.0f, 0.0f, 1.75f, 3.0f, 2.25f};
        
        // Bar colors
        barColors = {
            juce::Colour(0xff42a5f5),  // Blue
            juce::Colour(0xff66bb6a),  // Green
            juce::Colour(0xffffb74d),  // Orange
            juce::Colour(0xffef5350),  // Red
            juce::Colour(0xffab47bc),  // Purple
            juce::Colour(0xff26c6da),  // Cyan
            juce::Colour(0xffffa726)   // Amber
        };
    }
    
    juce::Label titleLabel;
    juce::ToggleButton weekButton;
    juce::ToggleButton monthButton;
    juce::ToggleButton yearButton;
    
    std::array<float, 7> weeklyData;
    std::array<juce::Colour, 7> barColors;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PracticeProgressComponent)
}; 