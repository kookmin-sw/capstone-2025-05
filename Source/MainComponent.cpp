#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(helloButton);
    helloButton.onClick = []()
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon, 
            juce::String::fromUTF8(u8"클릭"), 
            juce::String::fromUTF8(u8"버튼이 클릭되었습니다!"));
    };
    setSize(800, 600);
}

MainComponent::~MainComponent() {}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);
    g.setFont(20.0f);
    g.drawText("Welcome to MAPLE", getLocalBounds(),
               juce::Justification::centred, true);
}

void MainComponent::resized()
{
    helloButton.setBounds(getWidth()/2 - 50, getHeight()/2 - 20, 100, 40);
}