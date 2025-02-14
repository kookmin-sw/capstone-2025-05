#include "MainLayout.h"

MainLayout::MainLayout()
{
    addAndMakeVisible(leftSidebar);
    addAndMakeVisible(rightSidebar);
    addAndMakeVisible(mainPanel);
}

void MainLayout::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void MainLayout::resized()
{
    auto bounds = getLocalBounds();
    
    // 왼쪽 사이드바
    leftSidebar.setBounds(bounds.removeFromLeft(200));
    
    // 오른쪽 사이드바
    rightSidebar.setBounds(bounds.removeFromRight(200));
    
    // 메인 패널
    mainPanel.setBounds(bounds);
} 