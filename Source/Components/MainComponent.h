#pragma once
#include <JuceHeader.h>
#include "Navigation/SidebarComponent.h"
#include "Layout/HeaderComponent.h"
#include "Pages/Page.h"
#include "Pages/HomePage.h"
// #include "Pages/UploadPage.h"
// #include "Pages/LibraryPage.h"
// ... 다른 페이지 include

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    // 레이아웃 컴포넌트들
    HeaderComponent headerComponent;
    SidebarComponent sidebarComponent;
    std::unique_ptr<juce::Component> mainPanel;
    juce::Component footerComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};