#pragma once
#include <JuceHeader.h>
#include "../../Styles/MapleColours.h"

class MapleTextEditor : public juce::TextEditor,
                       private juce::Timer
{
public:
    MapleTextEditor();
    
    void focusGained(juce::Component::FocusChangeType cause) override;
    void focusLost(juce::Component::FocusChangeType cause) override;
    void paint(juce::Graphics& g) override;
    void setAnimationSpeed(float speed);

private:
    float easeOutCubic(float x);
    void startAnimation();
    void resetAnimation();
    void timerCallback() override;

    bool isAnimating = false;
    float animationProgress = 0.0f;
    float animationSpeed = 0.1f;
    double startTime = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapleTextEditor)
}; 