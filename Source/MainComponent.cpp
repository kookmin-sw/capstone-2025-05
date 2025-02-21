#include "MainComponent.h"
#include "HeaderComponent.h"
#include "MainActionComponent.h"
#include "ContentPanelComponent.h"
#include "BottomComponent.h"
#include "PracticeSongComponent.h"

MainComponent::MainComponent()
{
    // 컴포넌트들 초기화
    headerComponent = std::make_unique<HeaderComponent>();
    mainActionComponent = std::make_unique<MainActionComponent>(*this);
    contentPanelComponent = std::make_unique<ContentPanelComponent>();
    bottomComponent = std::make_unique<BottomComponent>();

    addAndMakeVisible(headerComponent.get());
    addAndMakeVisible(mainActionComponent.get());
    addAndMakeVisible(contentPanelComponent.get());
    addAndMakeVisible(bottomComponent.get());

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
    
    if (!practiceSongComponent)
        practiceSongComponent = std::make_unique<PracticeSongComponent>();
        
    addAndMakeVisible(practiceSongComponent.get());
    practiceSongComponent->setVisible(true);
    resized();
}