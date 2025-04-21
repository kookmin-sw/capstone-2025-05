#pragma once
#include <JuceHeader.h>

// Forward declaration instead of including the header
class GuitarPracticeComponent;

class TopBar : public juce::Component
{
public:
    TopBar(GuitarPracticeComponent &parent);
    ~TopBar() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    juce::TextButton recordButton{"Record"};
    juce::TextButton loadButton{"Load Original"};

    std::unique_ptr<juce::FileChooser> fileChooser;
    GuitarPracticeComponent &parentComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBar)
};