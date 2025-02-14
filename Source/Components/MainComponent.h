#pragma once
#include <JuceHeader.h>
#include "Layout/MainLayout.h"
#include "Layout/ProjectLayout.h"
#include "../Controllers/ApplicationController.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<ApplicationController> appController;
    std::unique_ptr<Component> currentLayout;
    
    void switchToMainLayout();
    void switchToProjectLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};