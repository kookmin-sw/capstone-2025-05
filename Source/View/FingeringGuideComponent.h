#pragma once
#include <JuceHeader.h>
#include "LookAndFeel/MapleTheme.h"

// Fingering guide display component
class FingeringGuideComponent : public juce::Component,
                               public juce::Timer
{
public:
    FingeringGuideComponent()
    {
        addAndMakeVisible(titleLabel);
        
        titleLabel.setText("Guitar Fingering Guide", juce::dontSendNotification);
        titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
        titleLabel.setJustificationType(juce::Justification::centred);
        
        // Start animation timer
        startTimer(30);
    }
    
    ~FingeringGuideComponent() override
    {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleTheme::getCardColour());
        
        auto bounds = getLocalBounds().reduced(15);
        bounds.removeFromTop(40); // Exclude title area
        
        // Guitar fretboard area
        auto fretboardArea = bounds.removeFromTop(bounds.getHeight() * 0.6f);
        g.setColour(MapleTheme::getCardColour().darker(0.2f));
        g.fillRoundedRectangle(fretboardArea.toFloat().reduced(10.0f), 5.0f);
        
        // Draw fretboard
        drawFretboard(g, fretboardArea.reduced(20, 10));
        
        // Hand position guide area
        auto handsArea = bounds;
        g.setColour(MapleTheme::getCardColour().darker(0.1f));
        g.fillRoundedRectangle(handsArea.toFloat().reduced(10.0f), 5.0f);
        
        // Draw hand position guide
        g.setColour(MapleTheme::getTextColour());
        g.setFont(15.0f);
        g.drawText("Hand Position Guide", handsArea.removeFromTop(25).reduced(10, 0), 
                  juce::Justification::centred, true);
                  
        // Split left/right hand areas
        auto leftHandArea = handsArea.removeFromLeft(handsArea.getWidth() / 2).reduced(10);
        auto rightHandArea = handsArea.reduced(10);
        
        // Left hand position text
        g.setFont(14.0f);
        g.drawText("Left Hand Position: 5th Fret", leftHandArea.removeFromTop(25), 
                  juce::Justification::centredLeft, true);
                  
        // Finger placement display (left hand)
        g.setFont(13.0f);
        leftHandArea.removeFromTop(10);
        g.drawText("1st Finger: 5th Fret A String", leftHandArea.removeFromTop(20), 
                  juce::Justification::centredLeft, true);
        g.drawText("2nd Finger: 7th Fret D String", leftHandArea.removeFromTop(20), 
                  juce::Justification::centredLeft, true);
        g.drawText("3rd Finger: 7th Fret G String", leftHandArea.removeFromTop(20), 
                  juce::Justification::centredLeft, true);
        g.drawText("4th Finger: 7th Fret B String", leftHandArea.removeFromTop(20), 
                  juce::Justification::centredLeft, true);
        
        // Right hand technique text
        g.drawText("Right Hand Technique: Picking", rightHandArea.removeFromTop(25), 
                  juce::Justification::centredLeft, true);
                  
        // Picking pattern display
        rightHandArea.removeFromTop(10);
        g.drawText("Down-Up-Down-Up Pattern", rightHandArea.removeFromTop(20), 
                  juce::Justification::centredLeft, true);
        g.drawText("16th Note Picking Speed", rightHandArea.removeFromTop(20), 
                  juce::Justification::centredLeft, true);
        g.drawText("Emphasize D String", rightHandArea.removeFromTop(20), 
                  juce::Justification::centredLeft, true);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(15);
        titleLabel.setBounds(bounds.removeFromTop(30));
    }
    
    void timerCallback() override
    {
        // Update animation
        animationOffset += 0.05f;
        if (animationOffset > 1000.0f)
            animationOffset = 0.0f;
            
        currentFingerPosition = (currentFingerPosition + 1) % 4;
        if (currentFingerPosition == 0)
            currentFret = (currentFret % 12) + 1;
            
        // Update UI
        repaint();
    }
    
private:
    void drawFretboard(juce::Graphics& g, juce::Rectangle<int> area)
    {
        // Fretboard background
        g.setColour(juce::Colour(0xff5d4037)); // Dark brown (rosewood)
        g.fillRect(area);
        
        const int numStrings = 6;
        const int numFrets = 12;
        
        float stringSpacing = area.getHeight() / static_cast<float>(numStrings - 1);
        float fretSpacing = area.getWidth() / static_cast<float>(numFrets);
        
        // Draw frets
        g.setColour(juce::Colour(0xffbdbdbd)); // Silver (fret wire)
        for (int i = 0; i <= numFrets; ++i)
        {
            float x = area.getX() + i * fretSpacing;
            g.drawLine(x, area.getY(), x, area.getBottom(), 2.0f);
        }
        
        // Draw fret markers (round inlays)
        g.setColour(juce::Colour(0xffe0e0e0));
        for (int fret : {3, 5, 7, 9, 12})
        {
            if (fret <= numFrets)
            {
                float x = area.getX() + (fret - 0.5f) * fretSpacing;
                float y = area.getCentreY();
                float diameter = stringSpacing * 0.8f;
                
                // 12th fret has double dots
                if (fret == 12)
                {
                    g.fillEllipse(x - diameter / 2, y - stringSpacing - diameter / 2, diameter, diameter);
                    g.fillEllipse(x - diameter / 2, y + stringSpacing - diameter / 2, diameter, diameter);
                }
                else
                {
                    g.fillEllipse(x - diameter / 2, y - diameter / 2, diameter, diameter);
                }
            }
        }
        
        // Draw strings
        for (int i = 0; i < numStrings; ++i)
        {
            float y = area.getY() + i * stringSpacing;
            float thickness = 3.0f - (i * 0.4f); // Thicker bass strings
            
            // String color
            g.setColour(juce::Colour(0xffd4d4d4).darker(0.2f * i));
            g.drawLine(area.getX(), y, area.getRight(), y, thickness);
        }
        
        // Show current finger positions (animation)
        const juce::Colour fingerColours[] = {
            juce::Colours::red.withAlpha(0.7f),    // Index finger
            juce::Colours::green.withAlpha(0.7f),  // Middle finger
            juce::Colours::blue.withAlpha(0.7f),   // Ring finger
            juce::Colours::yellow.withAlpha(0.7f)  // Pinky finger
        };
        
        // Calculate finger positions (animation)
        for (int i = 0; i < 4; ++i)
        {
            int finger = (i + currentFingerPosition) % 4;
            int string = (finger + 1) % numStrings;
            int fret = (currentFret + finger) % numFrets;
            
            float x = area.getX() + (fret - 0.5f) * fretSpacing;
            float y = area.getY() + string * stringSpacing;
            
            g.setColour(fingerColours[finger]);
            g.fillEllipse(x - 12, y - 12, 24, 24);
            
            g.setColour(juce::Colours::white);
            g.setFont(12.0f);
            g.drawText(juce::String(i + 1), x - 10, y - 10, 20, 20, juce::Justification::centred, false);
        }
    }
    
    juce::Label titleLabel;
    
    float animationOffset = 0.0f;
    int currentFingerPosition = 0;
    int currentFret = 5;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FingeringGuideComponent)
}; 