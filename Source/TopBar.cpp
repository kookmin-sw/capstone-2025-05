#include "TopBar.h"

TopBar::TopBar(PracticeSongComponent& practiceSong) : practiceSongComponent(practiceSong) {
    addAndMakeVisible(recordButton);
}

TopBar::~TopBar() {}

void TopBar::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void TopBar::resized() {
    auto bounds = getLocalBounds().reduced(5);
    recordButton.setBounds(bounds.removeFromLeft(80));
    loadButton.setBounds(bounds.removeFromLeft(100));
}