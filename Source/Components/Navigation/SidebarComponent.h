#pragma once
#include <JuceHeader.h>

class SidebarComponent : public juce::Component
{
public:
    SidebarComponent();
    ~SidebarComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    juce::TextButton homeButton;
    juce::TextButton libraryButton;
    juce::TextButton coursesButton;
    juce::TextButton exploreButton;
    juce::TextButton settingsButton;
    juce::TextButton profileButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
};