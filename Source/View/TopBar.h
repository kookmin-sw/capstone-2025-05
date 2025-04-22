#pragma once
#include <JuceHeader.h>
#include "IPanelComponent.h"

// Forward declaration instead of including the header
class GuitarPracticeComponent;

class TopBar : public juce::Component, public IPanelComponent
{
public:
    TopBar(GuitarPracticeComponent &parent);
    ~TopBar() override;

    // Component 오버라이드
    void paint(juce::Graphics &g) override;
    void resized() override;
    
    // IPanelComponent 인터페이스 구현
    void initialize() override;
    void updatePanel() override;
    void resetPanel() override;
    juce::Component* asComponent() override { return this; }

private:
    juce::TextButton recordButton{"Record"};
    juce::TextButton loadButton{"Load Original"};

    std::unique_ptr<juce::FileChooser> fileChooser;
    GuitarPracticeComponent &parentComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBar)
};