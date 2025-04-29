#pragma once
#include <JuceHeader.h>

// Forward declaration instead of including the header
class GuitarPracticeComponent;

class TopBar : public juce::Component
{
public:
    TopBar(GuitarPracticeComponent &parent);
    ~TopBar() override;

    // Component 오버라이드
    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    GuitarPracticeComponent &parentComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBar)
};