#pragma once
#include <JuceHeader.h>

// Forward declaration instead of including the header
class PracticeSongComponent;

class TopBar : public juce::Component
{
public:
    TopBar(PracticeSongComponent &parent);
    ~TopBar() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    juce::TextButton recordButton{"Record"};
    juce::TextButton loadButton{"Load Original"};

    std::unique_ptr<juce::FileChooser> fileChooser;
    PracticeSongComponent &parentComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBar)
};