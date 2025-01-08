#pragma once

#include <JuceHeader.h>
#include "Screen.h"

class StartScreenComponent : public juce::Component
{
public:
    StartScreenComponent(std::function<void(Screen)> screenChangeCallback);
    ~StartScreenComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    void drawButton(juce::Graphics& g, juce::Rectangle<int> bounds, 
                   const juce::String& text, bool isMouseOver, bool isMouseDown);
    void drawShadow(juce::Graphics& g, juce::Rectangle<float> bounds, float elevation);
    
    std::function<void(Screen)> screenChangeCallback;
    
    juce::TextButton recordButton;
    juce::TextButton uploadButton;
    
    juce::Font logoFont { juce::Font::bold };
    juce::Font materialFont;
    
    bool recordButtonMouseOver = false;
    bool uploadButtonMouseOver = false;
    bool recordButtonMouseDown = false;
    bool uploadButtonMouseDown = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StartScreenComponent)
}; 