#include "MainComponent.h"
#include "View/HeaderComponent.h"
#include "MainActionComponent.h"
#include "ContentPanelComponent.h"
#include "BottomComponent.h"
#include "PracticeSongComponent.h"
#include "Controller/ContentController.h"
#include "Model/ContentModel.h"

MainComponent::MainComponent()
{
    // 1. 오디오 시스템 초기화 (이후 AudioController로 이동 예정)
    deviceManager.initialiseWithDefaultDevices(2, 2);

    // 2. UI 컴포넌트 초기화 (View)
    headerComponent = std::make_unique<HeaderComponent>();
    mainActionComponent = std::make_unique<MainActionComponent>(*this);
    contentPanelComponent = std::make_unique<ContentPanelComponent>();
    bottomComponent = std::make_unique<BottomComponent>();
    practiceSongComponent = std::make_unique<PracticeSongComponent>(*this);

    // 3. Model, Controller 초기화
    auto contentModel = std::make_shared<ContentModel>();
    contentController = std::make_shared<ContentController>(*contentModel, *this, *practiceSongComponent);
    
    // 곡 선택 이벤트 리스너 등록 (MainComponent가 아닌 ContentController가 리스너로 등록)
    contentPanelComponent->addSongSelectedListener(contentController.get());
    
    // 데이터 초기화
    contentController->initializeData();

    // 4. UI 컴포넌트 추가
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

// 화면 전환 메서드 (View 관점에서)
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
    
    practiceSongComponent->setVisible(true);
    resized();
}