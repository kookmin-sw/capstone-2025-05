#pragma once
#include <JuceHeader.h>
#include "../../UI/Common/Button/MapleButton.h"

class SidebarComponent : public juce::Component
{
public:
    SidebarComponent();
    ~SidebarComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
};