#include "SidebarComponent.h"
#include "../../UI/Styles/MapleColours.h"
#include "../../Utils/Constants.h"

SidebarComponent::SidebarComponent()
{
}

SidebarComponent::~SidebarComponent()
{
}

void SidebarComponent::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();

    // 둥근 모서리의 배경 그리기
    g.setColour(MapleColours::currentTheme.panel);
    g.fillRoundedRectangle(bounds.toFloat(), 10.0f); // 10.0f는 모서리의 둥글기 정도
}

void SidebarComponent::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    const int buttonHeight = 40;
    const int buttonSpacing = 10;
}