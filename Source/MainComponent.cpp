#include "MainComponent.h"

MainComponent::MainComponent()
{
    // 컴포넌트들 초기화
    headerComponent = std::make_unique<HeaderComponent>();
    mainActionComponent = std::make_unique<MainActionComponent>();
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
    
    // 헤더 영역 (상단 15% 차지)
    headerComponent->setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.15)));
    
    // 메인 액션 영역 (다음 15% 차지)
    mainActionComponent->setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.15)));
    
    // 하단 영역 (하단 30% 차지)
    bottomComponent->setBounds(bounds.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.3)));
    
    // 콘텐츠 패널 영역 (나머지 공간)
    contentPanelComponent->setBounds(bounds);
}