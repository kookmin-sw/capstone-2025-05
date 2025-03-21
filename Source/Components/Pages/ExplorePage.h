#pragma once
#include "Page.h"

class ExplorePage : public Page
{
public:
    ExplorePage() = default;
    juce::String getPageName() const override { return "Explore"; }

    void paint(juce::Graphics& g) override
    {
        Page::paint(g);
        g.setColour(MapleColours::currentTheme.text);
        drawSafeText(g, "Explore Page", getLocalBounds().reduced(20));
    }
}; 