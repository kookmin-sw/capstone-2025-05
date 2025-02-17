#pragma once
#include <JuceHeader.h>

class HeaderComponent : public juce::Component
{
public:
    HeaderComponent();
    void resized() override;

private:
    juce::Label title;
    std::vector<std::unique_ptr<juce::TextButton>> menuButtons;
    juce::TextButton profileButton;
    juce::TextButton settingsButton;
}; 