#pragma once
#include "Page.h"

class LibraryPage : public Page
{
public:
    LibraryPage() = default;
    juce::String getPageName() const override { return "Library"; }

    void paint(juce::Graphics& g) override
    {
        Page::paint(g);
        g.setColour(MapleColours::currentTheme.text);
        drawSafeText(g, "Library Page", getLocalBounds().reduced(20));
    }
}; 