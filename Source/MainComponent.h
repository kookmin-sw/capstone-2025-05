#pragma once

#include <JuceHeader.h>
#include "Screen.h"
#include "StartScreenComponent.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void showScreen(Screen screen);

private:
    std::unique_ptr<juce::Component> currentScreen;
    Screen currentScreenType;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
