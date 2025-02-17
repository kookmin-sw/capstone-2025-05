#pragma once
#include <JuceHeader.h>
#include "HeaderComponent.h"
#include "MainActionComponent.h"
#include "ContentPanelComponent.h"
#include "BottomComponent.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<HeaderComponent> headerComponent;
    std::unique_ptr<MainActionComponent> mainActionComponent;
    std::unique_ptr<ContentPanelComponent> contentPanelComponent;
    std::unique_ptr<BottomComponent> bottomComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};