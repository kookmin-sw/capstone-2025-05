#pragma once
#include <JuceHeader.h>
#include "Navigation/SidebarComponent.h"
#include "Pages/HomePage.h"
// #include "Pages/LibraryPage.h"
// #include "Pages/CoursesPage.h"
// #include "Pages/ExplorePage.h"
// #include "Pages/SettingsPage.h"
// #include "Pages/ProfilePage.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    SidebarComponent sidebar;
    std::unique_ptr<juce::Component> currentPage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};