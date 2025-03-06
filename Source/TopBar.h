#pragma once
#include <JuceHeader.h>
#include "PracticeSongComponent.h" // MainComponent 포함

class TopBar : public juce::Component {
public:
    TopBar(PracticeSongComponent& practiceSongComponent);
    ~TopBar() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::TextButton recordButton{"Record"};
    juce::TextButton loadButton{"Load Original"};

    std::unique_ptr<juce::FileChooser> fileChooser;
    PracticeSongComponent& practiceSongComponent;

    // JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBar)
};