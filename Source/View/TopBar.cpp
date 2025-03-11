#include "TopBar.h"
#include "../PracticeSongComponent.h" // Now include the full header here

TopBar::TopBar(PracticeSongComponent &parent)
    : parentComponent(parent)
{
    addAndMakeVisible(recordButton);
}

TopBar::~TopBar() {}

void TopBar::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void TopBar::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    recordButton.setBounds(bounds.removeFromLeft(80));
    loadButton.setBounds(bounds.removeFromLeft(100));
}