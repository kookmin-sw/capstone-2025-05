#include "MainComponent.h"
#include "../Utils/Constants.h"
#include "../UI/Styles/MapleColours.h"
#include "../Controllers/ApplicationController.h"
#include "Layout/MainLayout.h"
#include "Layout/ProjectLayout.h"

MainComponent::MainComponent()
{
    appController = std::make_unique<ApplicationController>();
    
    // 프로젝트 상태 변경 이벤트 처리
    appController->onProjectStateChanged = [this]() {
        if (appController->isProjectOpen())
            switchToProjectLayout();
        else
            switchToMainLayout();
    };
    
    // 초기 레이아웃 설정
    switchToMainLayout();
}

MainComponent::~MainComponent()
{
    currentLayout.reset();
    appController.reset();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void MainComponent::resized()
{
    if (currentLayout)
        currentLayout->setBounds(getLocalBounds());
}

void MainComponent::switchToMainLayout()
{
    auto newLayout = std::make_unique<MainLayout>();
    addAndMakeVisible(*newLayout);
    currentLayout = std::move(newLayout);
    resized();
}

void MainComponent::switchToProjectLayout()
{
    auto newLayout = std::make_unique<ProjectLayout>();
    addAndMakeVisible(*newLayout);
    currentLayout = std::move(newLayout);
    resized();
}