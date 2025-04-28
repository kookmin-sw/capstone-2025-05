#include "MainComponent.h"
#include "GuitarPracticeComponent.h"
#include "Controller/ContentController.h"
#include "Model/ContentModel.h"

MainComponent::MainComponent()
{
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
    
    // 곡 그리드 뷰 설정
    featuredSongsView = std::make_unique<SongGridView>("Featured Songs");
    featuredSongsView->setOnSongSelectedCallback([this](juce::String songId) {
        handleSongSelected(songId);
    });
    mainContentContainer.addAndMakeVisible(featuredSongsView.get());
    
    recentlyPlayedView = std::make_unique<SongGridView>("Recently Played");
    recentlyPlayedView->setOnSongSelectedCallback([this](juce::String songId) {
        handleSongSelected(songId);
    });
    mainContentContainer.addAndMakeVisible(recentlyPlayedView.get());
    
    recommendedView = std::make_unique<SongGridView>("Recommended For You");
    recommendedView->setOnSongSelectedCallback([this](juce::String songId) {
        handleSongSelected(songId);
    });
    mainContentContainer.addAndMakeVisible(recommendedView.get());
    
    // 연습 컴포넌트 초기화
    guitarPracticeComponent = std::make_unique<GuitarPracticeComponent>(*this);
    addChildComponent(guitarPracticeComponent.get());  // 초기에는 숨김 상태
    
    // 모델, 컨트롤러 초기화
    auto contentModel = std::make_shared<ContentModel>();
    contentController = std::make_shared<ContentController>(*contentModel, *this, *guitarPracticeComponent);
    
    // 데이터 초기화
    contentController->initializeData();
    
    // 테스트용 더미 데이터 추가
    addDummySongs();

    setSize(1400, 900);
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
        
        // 메인 콘텐츠 컨테이너 크기 설정
        auto containerWidth = bounds.getWidth() - 20; // 스크롤바 공간 고려
        auto containerHeight = 0;
        
        // 그리드 뷰 배치
        auto gridHeight = 350;
        
        if (featuredSongsView)
        {
            featuredSongsView->setBounds(0, containerHeight, containerWidth, gridHeight);
            containerHeight += gridHeight;
        }
        
        if (recentlyPlayedView)
        {
            recentlyPlayedView->setBounds(0, containerHeight, containerWidth, gridHeight);
            containerHeight += gridHeight;
        }
        
        if (recommendedView)
        {
            recommendedView->setBounds(0, containerHeight, containerWidth, gridHeight);
            containerHeight += gridHeight;
        }
        
        // 컨테이너 최종 크기 설정
        mainContentContainer.setBounds(0, 0, containerWidth, containerHeight);
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

void MainComponent::handleSongSelected(const juce::String& songId)
{
    // 컨트롤러를 통해 곡 로드 및 연습 화면으로 전환
    if (guitarPracticeComponent && contentController)
    {
        showPracticeScreen();
        guitarPracticeComponent->loadSong(songId);
    }
}

// 테스트용 더미 데이터 추가 메서드
void MainComponent::addDummySongs()
{
    // 특성 곡 (Featured Songs)
    for (int i = 1; i <= 6; ++i)
    {
        SongCard::SongInfo song;
        song.id = "featured_" + juce::String(i);
        song.title = "Featured Song " + juce::String(i);
        song.artist = "Artist " + juce::String(i);
        song.albumTitle = "Album " + juce::String(i);
        song.duration = "3:0" + juce::String(i);
        
        featuredSongsView->addCard(song);
    }
    
    // 최근 재생 (Recently Played)
    for (int i = 1; i <= 6; ++i)
    {
        SongCard::SongInfo song;
        song.id = "recent_" + juce::String(i);
        song.title = "Recent Song " + juce::String(i);
        song.artist = "Artist " + juce::String(i + 10);
        song.albumTitle = "Album " + juce::String(i + 10);
        song.duration = "4:1" + juce::String(i);
        
        recentlyPlayedView->addCard(song);
    }
    
    // 추천 곡 (Recommended)
    for (int i = 1; i <= 6; ++i)
    {
        SongCard::SongInfo song;
        song.id = "recommended_" + juce::String(i);
        song.title = "Recommended Song " + juce::String(i);
        song.artist = "Artist " + juce::String(i + 20);
        song.albumTitle = "Album " + juce::String(i + 20);
        song.duration = "2:5" + juce::String(i);
        
        recommendedView->addCard(song);
    }
}