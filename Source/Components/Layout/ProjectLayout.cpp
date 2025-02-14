#include "ProjectLayout.h"

ProjectLayout::ProjectLayout()
{
    addAndMakeVisible(mainPanel);
}

void ProjectLayout::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void ProjectLayout::resized()
{
    // 프로젝트 화면에서는 전체 영역을 메인 패널이 사용
    mainPanel.setBounds(getLocalBounds());
} 