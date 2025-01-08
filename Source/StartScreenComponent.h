#pragma once

#include <JuceHeader.h>
#include "Screen.h"

class StartScreenComponent : public juce::Component,
                           public juce::MouseListener
{
public:
    StartScreenComponent(std::function<void(Screen)> screenChangeCallback);
    ~StartScreenComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    // Mouse event handlers
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

private:
    void drawButton(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& text, bool isMouseOver);
    
    std::function<void(Screen)> screenChangeCallback;
    
    juce::TextButton recordButton;
    juce::TextButton uploadButton;
    
    // Font for logo
    juce::Font logoFont { 72.0f, juce::Font::bold };
    
    // Mouse states for custom button drawing
    bool recordButtonMouseOver = false;
    bool uploadButtonMouseOver = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StartScreenComponent)
}; 