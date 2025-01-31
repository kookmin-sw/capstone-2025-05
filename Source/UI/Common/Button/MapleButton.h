#pragma once
#include <JuceHeader.h>

class MapleButton : public juce::Component
{
public:
    MapleButton(const juce::String &buttonText = {});

    void setText(const juce::String &newText);
    void setOnClick(std::function<void()> callback);

    void paint(juce::Graphics &g) override;
    void resized() override;

    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

private:
    juce::String text;
    bool isMouseOver = false;
    bool isMouseDown = false;
    std::function<void()> onClick;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapleButton)
};