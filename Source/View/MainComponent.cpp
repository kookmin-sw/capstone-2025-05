#include "MainComponent.h"
#include "GuitarPracticeComponent.h"
#include "Controller/ContentController.h"
#include "Model/ContentModel.h"

MainComponent::MainComponent()
{
    DBG("MainComponent - constructor start");

    // 테마 설정
    setLookAndFeel(&theme);
    
    // 오디오 시스템 초기화
    deviceManager.initialiseWithDefaultDevices(2, 2);

    // UI 컴포넌트 초기화
    sidebarComponent = std::make_unique<SidebarComponent>();
    sidebarComponent->setOnMenuItemSelectedCallback([this](SidebarComponent::ItemType menuItem) {
        handleMenuItemSelected(menuItem);
    });
    addAndMakeVisible(sidebarComponent.get());
    
    headerComponent = std::make_unique<SearchHeaderComponent>();
    headerComponent->setOnSearchSubmittedCallback([this](juce::String query) {
        // 검색 기능 구현
        DBG("Search query: " + query);
    });
    addAndMakeVisible(headerComponent.get());
    
    // 메인 콘텐츠 스크롤 뷰 설정
    mainContentViewport.setViewedComponent(&mainContentContainer, false);
    mainContentViewport.setScrollBarsShown(true, false);
    addAndMakeVisible(mainContentViewport);
    
    // 콘텐츠 패널 컴포넌트 초기화
    contentPanelComponent = std::make_unique<ContentPanelComponent>();
    mainContentContainer.addAndMakeVisible(contentPanelComponent.get());
    
    // 연습 컴포넌트 초기화
    guitarPracticeComponent = std::make_unique<GuitarPracticeComponent>(*this);
    addChildComponent(guitarPracticeComponent.get());  // 초기에는 숨김 상태
    
    // 모델, 컨트롤러 초기화
    contentModel = std::make_shared<ContentModel>();
    contentController = std::make_shared<ContentController>(*contentModel, *this, *guitarPracticeComponent);
    
    // GuitarPracticeController에 ContentController 설정
    guitarPracticeComponent->getController()->setContentController(contentController);
    
    DBG("MainComponent - MVC components initialized");
    
    // ContentPanelComponent에 ContentController 설정
    contentPanelComponent->setContentController(contentController.get());
    
    // ContentController를 리스너로 등록
    contentPanelComponent->addSongSelectedListener(contentController.get());
    
    // 데이터 초기화 및 화면 업데이트
    DBG("MainComponent - Initializing content panel");
    contentPanelComponent->initialize();

    setSize(1400, 900);
    
    DBG("MainComponent - constructor complete");
}

MainComponent::~MainComponent()
{
    prepareToClose();
    setLookAndFeel(nullptr);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(theme.getBackgroundColour());
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    if (guitarPracticeComponent && guitarPracticeComponent->isVisible())
    {
        // 연습 화면 표시 중일 때
        guitarPracticeComponent->setBounds(bounds);
    }
    else
    {
        // 메인 화면이 표시 중일 때
        
        // 사이드바 (좌측)
        auto sidebarWidth = 220;
        sidebarComponent->setBounds(bounds.removeFromLeft(sidebarWidth));
        
        // 헤더 (상단)
        auto headerHeight = 70;
        headerComponent->setBounds(bounds.removeFromTop(headerHeight));
        
        // 메인 콘텐츠 영역 (나머지)
        mainContentViewport.setBounds(bounds);
        
        // 콘텐츠 패널 크기 설정 (동적으로 필요한 높이 계산)
        int contentHeight = 1200; // 더 큰 기본 높이로 설정 (1000->1200)
        contentPanelComponent->setBounds(0, 0, bounds.getWidth() - 20, contentHeight);
        
        // 실제 콘텐츠 크기에 맞춰 컨테이너 크기 조정
        mainContentContainer.setBounds(0, 0, bounds.getWidth() - 20, contentHeight);
    }
}

void MainComponent::showMainScreen()
{
    sidebarComponent->setVisible(true);
    headerComponent->setVisible(true);
    mainContentViewport.setVisible(true);
    
    guitarPracticeComponent->setVisible(false);
    
    resized();
}

void MainComponent::showPracticeScreen()
{
    sidebarComponent->setVisible(false);
    headerComponent->setVisible(false);
    mainContentViewport.setVisible(false);
    
    guitarPracticeComponent->setVisible(true);
    resized();
}

void MainComponent::prepareToClose()
{
    // 오디오 프로세스 중지
    deviceManager.closeAudioDevice();
    
    // 컴포넌트 정리
    if (guitarPracticeComponent)
    {
        removeChildComponent(guitarPracticeComponent.get());
        guitarPracticeComponent = nullptr;
    }
    
    // 컨트롤러 정리
    contentController = nullptr;
}

void MainComponent::handleMenuItemSelected(SidebarComponent::ItemType menuItem)
{
    switch (menuItem)
    {
        case SidebarComponent::ItemType::Practice:
            showPracticeScreen();
            break;
            
        case SidebarComponent::ItemType::Home:
        case SidebarComponent::ItemType::Library:
        case SidebarComponent::ItemType::Favorites:
        case SidebarComponent::ItemType::History:
        case SidebarComponent::ItemType::Settings:
            showMainScreen();
            break;
    }
}