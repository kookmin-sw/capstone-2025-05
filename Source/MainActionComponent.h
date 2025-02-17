#pragma once
#include <JuceHeader.h>

class MainActionComponent : public juce::Component
{
public:
    MainActionComponent();
    void resized() override;

private:
    juce::TextButton startButton;
    juce::Label subtitle;
}; 