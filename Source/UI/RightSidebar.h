#pragma once
#include <JuceHeader.h>

class RightSidebar : public juce::Component
{
public:
    RightSidebar()
    {
        setSize(200, 600);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::lightgrey);
    }

    void resized() override {}
}; 