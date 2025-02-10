#pragma once
#include "Page.h"

class CoursesPage : public Page
{
public:
    CoursesPage() = default;
    juce::String getPageName() const override { return "Courses"; }

    void paint(juce::Graphics& g) override
    {
        Page::paint(g);
        g.setColour(MapleColours::currentTheme.text);
        drawSafeText(g, "Courses Page", getLocalBounds().reduced(20));
    }
}; 