#pragma once
#include <JuceHeader.h>

class LeftSidebar : public juce::Component
{
public:
    LeftSidebar()
    {
        setSize(200, 600);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::lightgrey);
    }

    void resized() override {}
}; 