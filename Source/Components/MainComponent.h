#pragma once
#include <JuceHeader.h>
#include "Navigation/SidebarComponent.h"
#include "Layout/HeaderComponent.h"
#include "Pages/Page.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    HeaderComponent headerComponent;
    SidebarComponent sidebarComponent;
    std::unique_ptr<Page> mainPanel;
    juce::Component footerComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};