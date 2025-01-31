#pragma once
#include <JuceHeader.h>
#include "../../Styles/MapleTypography.h"

class MapleButton : public juce::Component
{
public:
    enum ColourIds
    {
        backgroundColourId = 0x1000100,
        textColourId = 0x1000101
    };

    MapleButton(const juce::String &buttonText = {});

    void setText(const juce::String &newText);
    void setOnClick(std::function<void()> callback);
    void setFont(const juce::Font &newFont);

    void paint(juce::Graphics &g) override;
    void resized() override;

    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

private:
    juce::String text;
    juce::Font font{juce::Font(16.0f, juce::Font::bold)}; // 기본 폰트로 변경
    bool isMouseOver = false;
    bool isMouseDown = false;
    std::function<void()> onClick;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapleButton)
};