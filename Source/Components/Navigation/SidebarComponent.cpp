#include "SidebarComponent.h"
#include "../../Utils/Constants.h"

SidebarComponent::SidebarComponent()
{
    // 버튼 초기화 및 설정
    addAndMakeVisible(homeButton);
    homeButton.setButtonText("Home");

    addAndMakeVisible(libraryButton);
    libraryButton.setButtonText("Library");

    addAndMakeVisible(coursesButton);
    coursesButton.setButtonText("Courses");

    addAndMakeVisible(exploreButton);
    exploreButton.setButtonText("Explore");

    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Settings");

    addAndMakeVisible(profileButton);
    profileButton.setButtonText("Profile");
}

SidebarComponent::~SidebarComponent()
{
}

void SidebarComponent::paint(juce::Graphics &g)
{
    g.fillAll(Constants::Colors::background);
}

void SidebarComponent::resized()
{
    auto bounds = getLocalBounds();
    const int buttonHeight = 40;
    const int buttonMargin = 5;

    // 각 버튼의 위치 설정
    homeButton.setBounds(bounds.removeFromTop(buttonHeight).reduced(buttonMargin));
    libraryButton.setBounds(bounds.removeFromTop(buttonHeight).reduced(buttonMargin));
    coursesButton.setBounds(bounds.removeFromTop(buttonHeight).reduced(buttonMargin));
    exploreButton.setBounds(bounds.removeFromTop(buttonHeight).reduced(buttonMargin));

    // Settings와 Profile 버튼은 하단에 배치
    auto bottomBounds = bounds.removeFromBottom(buttonHeight * 2);
    settingsButton.setBounds(bottomBounds.removeFromBottom(buttonHeight).reduced(buttonMargin));
    profileButton.setBounds(bottomBounds.removeFromBottom(buttonHeight).reduced(buttonMargin));
}