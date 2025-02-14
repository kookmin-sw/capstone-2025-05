#pragma once
#include <JuceHeader.h>

class MainPanel : public juce::Component
{
public:
    MainPanel()
    {
        setSize(600, 400);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::white);
    }

    void resized() override {}
}; 