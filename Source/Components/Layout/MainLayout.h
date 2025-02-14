#pragma once

#include <JuceHeader.h>
#include "../../UI/LeftSidebar.h"
#include "../../UI/RightSidebar.h"
#include "../../UI/MainPanel.h"

class MainLayout : public juce::Component
{
public:
    MainLayout();
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    LeftSidebar leftSidebar;
    RightSidebar rightSidebar;
    MainPanel mainPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainLayout)
}; 