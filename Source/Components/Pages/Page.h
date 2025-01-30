#pragma once
#include <JuceHeader.h>
#include "../../UI/Styles/MapleColours.h"

class Page : public juce::Component
{
public:
    Page() = default;
    ~Page() override = default;

    virtual juce::String getPageName() const = 0;

protected:
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds();

        // 둥근 모서리의 배경 그리기
        g.setColour(MapleColours::currentTheme.panel);
        g.fillRoundedRectangle(bounds.toFloat(), 10.0f);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Page)
};