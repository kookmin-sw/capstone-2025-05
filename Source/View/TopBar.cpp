#include "TopBar.h"
#include "GuitarPracticeComponent.h" // Now include the full header here

TopBar::TopBar(GuitarPracticeComponent &parent)
    : parentComponent(parent)
{
}

TopBar::~TopBar() {}

void TopBar::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::darkgrey);
    
    // 제목 표시
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Guitar Practice Mode", getLocalBounds(), juce::Justification::centred, true);
}

void TopBar::resized()
{
}