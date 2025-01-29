#pragma once

#include <JuceHeader.h>
#include "Screen.h"

class RoundedTextEditorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawTextEditorOutline(juce::Graphics &g, int width, int height, juce::TextEditor &textEditor) override
    {
        if (dynamic_cast<juce::AlertWindow *>(textEditor.getParentComponent()) == nullptr)
        {
            if (textEditor.isEnabled())
            {
                g.setColour(textEditor.findColour(juce::TextEditor::outlineColourId));
                g.drawRoundedRectangle(0, 0, width, height, 10.0f, 1.0f);
            }
        }
    }

    void fillTextEditorBackground(juce::Graphics &g, int width, int height, juce::TextEditor &textEditor) override
    {
        g.setColour(textEditor.findColour(juce::TextEditor::backgroundColourId));
        g.fillRoundedRectangle(0, 0, width, height, 10.0f);
    }
};

class StartScreenComponent : public juce::Component
{
public:
    StartScreenComponent(std::function<void(Screen)> screenChangeCallback);
    ~StartScreenComponent() override;

    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseMove(const juce::MouseEvent &event) override;

private:
    void drawRightPanel(juce::Graphics &g, juce::Rectangle<int> bounds);
    void drawLoginButton(juce::Graphics &g);

    bool isMouseOverButton(const juce::Point<float> &position) const;

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

    juce::TextEditor idField;
    juce::TextEditor passwordField;

    RoundedTextEditorLookAndFeel roundedLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StartScreenComponent)
};