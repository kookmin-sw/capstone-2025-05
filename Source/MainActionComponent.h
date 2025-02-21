#pragma once
#include <JuceHeader.h>

class MainComponent;  // 전방선언 추가

class MainActionComponent : public juce::Component
{
public:
    MainActionComponent(MainComponent& mainComp);
    void resized() override;

private:
    MainComponent& mainComponent;
    juce::TextButton startButton;
    juce::Label subtitle;
}; 