#pragma once
#include <JuceHeader.h>
#include "../../UI/Styles/MapleColours.h"

class HeaderComponent : public juce::Component
{
public:
    HeaderComponent() = default;
    ~HeaderComponent() override = default;

    void paint(juce::Graphics &g) override
    {
        g.setColour(MapleColours::currentTheme.panel);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderComponent)
};