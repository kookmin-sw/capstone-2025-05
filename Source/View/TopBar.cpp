#include "TopBar.h"
#include "GuitarPracticeComponent.h" // Now include the full header here
#include "LookAndFeel/MapleTheme.h"

TopBar::TopBar(GuitarPracticeComponent &parent)
    : parentComponent(parent)
{
}

TopBar::~TopBar() {}

void TopBar::paint(juce::Graphics &g)
{
    g.fillAll(MapleTheme::getCardColour());
    
    // 제목 표시
    g.setColour(MapleTheme::getTextColour());
    g.setFont(18.0f);
    g.drawText("Guitar Practice Mode", getLocalBounds(), juce::Justification::centred, true);
}

void TopBar::resized()
{
}