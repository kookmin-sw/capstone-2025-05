#pragma once

#include <JuceHeader.h>

class SidebarButton : public juce::Component
{
public:
    SidebarButton(const juce::String& name, const juce::String& iconPath);
    void paint(juce::Graphics& g) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    std::function<void()> onClick;

private:
    juce::String name;
    juce::Image icon;
    bool isMouseOver = false;
}; 