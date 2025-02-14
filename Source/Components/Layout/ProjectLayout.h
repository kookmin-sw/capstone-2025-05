#pragma once
#include <JuceHeader.h>
#include "../../UI/MainPanel.h"

class ProjectLayout : public juce::Component
{
public:
    ProjectLayout();
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MainPanel mainPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectLayout)
}; 