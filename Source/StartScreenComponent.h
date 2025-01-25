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
    void mouseMove(const juce::MouseEvent& event) override;

private:
    void drawRightPanel(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawLoginButton(juce::Graphics& g);
    
    bool isMouseOverButton(const juce::Point<float>& position) const;
    
    std::function<void(Screen)> screenChangeCallback;
    juce::Image logoImage;
    
    juce::Font titleFont;
    juce::Font descriptionFont;
    
    const juce::String projectDescription = 
        "MAPLE is a music education assistant program\n"
        "that records and analyzes musical performances\n"
        "to provide feedback for better practice.";

    juce::Rectangle<int> loginButtonBounds;
    bool isLoginButtonMouseOver = false;
    bool isLoginButtonDown = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StartScreenComponent)
}; 