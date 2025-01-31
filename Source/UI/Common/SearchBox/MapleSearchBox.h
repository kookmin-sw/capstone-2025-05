#pragma once
#include <JuceHeader.h>
#include "../../Styles/MapleColours.h"
#include "../../Styles/MapleTypography.h"

class MapleSearchBox : public juce::TextEditor
{
public:
    MapleSearchBox();
    ~MapleSearchBox() override;

    void paint(juce::Graphics &g) override;

private:
    class CustomLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawTextEditorOutline(juce::Graphics &g, int width, int height, juce::TextEditor &textEditor) override;
    };

    std::unique_ptr<juce::Drawable> searchIcon;
    CustomLookAndFeel customLookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapleSearchBox)
};