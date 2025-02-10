#pragma once
#include "Page.h"
#include "../../UI/Styles/MapleTypography.h"

class UploadPage : public Page
{
public:
    UploadPage() = default;
    juce::String getPageName() const override { return "Upload"; }

    void paint(juce::Graphics& g) override
    {
        Page::paint(g);
        g.setColour(MapleColours::currentTheme.text);
        drawSafeText(g, "Upload Page", getLocalBounds().reduced(20));
    }
}; 