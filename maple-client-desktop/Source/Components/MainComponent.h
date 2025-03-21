#pragma once
#include <JuceHeader.h>
#include "Navigation/SidebarComponent.h"
#include "Layout/HeaderComponent.h"
#include "Pages/Page.h"
#include "Pages/HomePage.h"
#include "Pages/UploadPage.h"
#include "Pages/LibraryPage.h"
#include "Pages/CoursesPage.h"
#include "Pages/ExplorePage.h"
#include "ProjectModeComponent.h"

class MainComponent : public juce::Component
{
public:
    enum class Mode
    {
        Normal,
        Project
    };

    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // 페이지 전환 함수 추가
    void switchToPage(const juce::String& pageName);

    void setMode(Mode newMode);

private:
    Mode currentMode = Mode::Normal;
    std::unique_ptr<juce::Component> projectModeComponent;
    HeaderComponent headerComponent;
    SidebarComponent sidebarComponent;
    std::unique_ptr<Page> mainPanel;
    juce::Component footerComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};