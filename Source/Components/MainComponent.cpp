#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(sidebar);
    currentPage = std::make_unique<HomePage>();
    addAndMakeVisible(currentPage.get());
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    sidebar.setBounds(bounds.removeFromLeft(200));
    currentPage->setBounds(bounds);
}