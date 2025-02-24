#include "MainComponent.h"
#include "HeaderComponent.h"
#include "MainActionComponent.h"
#include "ContentPanelComponent.h"
#include "BottomComponent.h"
#include "PracticeSongComponent.h"

MainComponent::MainComponent()
{
    // 1. 오디오 시스템 초기화
    deviceManager.initialiseWithDefaultDevices(2, 2);

    // 2. UI 컴포넌트 초기화
    headerComponent = std::make_unique<HeaderComponent>();
    mainActionComponent = std::make_unique<MainActionComponent>(*this);
    contentPanelComponent = std::make_unique<ContentPanelComponent>();
    bottomComponent = std::make_unique<BottomComponent>();
    practiceSongComponent = std::make_unique<PracticeSongComponent>();  // 미리 생성

    addAndMakeVisible(headerComponent.get());
    addAndMakeVisible(mainActionComponent.get());
    addAndMakeVisible(contentPanelComponent.get());
    addAndMakeVisible(bottomComponent.get());
    addChildComponent(practiceSongComponent.get());  // 숨겨진 상태로 추가

    setSize(1920, 1200);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    if (practiceSongComponent && practiceSongComponent->isVisible())
    {
        // 연습 화면이 표시 중일 때
        practiceSongComponent->setBounds(bounds);
    }
    else
    {
        // 메인 화면이 표시 중일 때
        headerComponent->setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.15)));
        mainActionComponent->setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.15)));
        bottomComponent->setBounds(bounds.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.3)));
        contentPanelComponent->setBounds(bounds);
    }
}

void MainComponent::showMainScreen()
{
    headerComponent->setVisible(true);
    mainActionComponent->setVisible(true);
    contentPanelComponent->setVisible(true);
    bottomComponent->setVisible(true);
    
    if (practiceSongComponent)
        practiceSongComponent->setVisible(false);
        
    resized();
}

void MainComponent::showPracticeScreen()
{
    headerComponent->setVisible(false);
    mainActionComponent->setVisible(false);
    contentPanelComponent->setVisible(false);
    bottomComponent->setVisible(false);
    
    practiceSongComponent->setVisible(true);  // 이미 생성된 컴포넌트를 표시
    resized();
}