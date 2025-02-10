#include "MainComponent.h"
#include "../Utils/Constants.h"
#include "../UI/Styles/MapleColours.h"
#include "Pages/HomePage.h"
#include "Pages/UploadPage.h"
#include "Pages/LibraryPage.h"
#include "Pages/CoursesPage.h"
#include "Pages/ExplorePage.h"

MainComponent::MainComponent()
{
    // 메인 UI 구성 요소 초기화
    addAndMakeVisible(headerComponent);
    addAndMakeVisible(sidebarComponent);
    addAndMakeVisible(footerComponent);

    // 사이드바 메뉴 클릭 이벤트 연결
    sidebarComponent.onMenuItemClick = [this](const juce::String& pageName) {
        switchToPage(pageName);
    };

    // 기본 페이지로 Home 설정
    mainPanel = std::make_unique<HomePage>();
    if (mainPanel != nullptr)
    {
        addAndMakeVisible(*mainPanel);
    }
}

MainComponent::~MainComponent()
{
    // 컴포넌트 제거 순서를 명시적으로 제어
    if (mainPanel != nullptr)
    {
        removeChildComponent(mainPanel.get());
        mainPanel.reset();
    }
}

void MainComponent::paint(juce::Graphics& g)
{
    // 메인 UI 배경
    g.fillAll(MapleColours::currentTheme.background);
}

void MainComponent::resized()
{
    const int margin = 10;
    auto bounds = getLocalBounds().reduced(margin);

    // 헤더 영역 설정 (상단)
    headerComponent.setBounds(bounds.removeFromTop(Constants::HEADER_HEIGHT));
    bounds.removeFromTop(margin); // 헤더와 메인 영역 사이 여백

    // 푸터 영역 설정 (하단)
    auto footerBounds = bounds.removeFromBottom(Constants::FOOTER_HEIGHT);
    bounds.removeFromBottom(margin); // 메인 영역과 푸터 사이 여백
    footerComponent.setBounds(footerBounds);

    // 남은 공간을 메인 영역으로 사용
    auto mainArea = bounds;

    // 사이드바 영역 설정 (좌측)
    sidebarComponent.setBounds(mainArea.removeFromLeft(Constants::SIDEBAR_WIDTH));
    mainArea.removeFromLeft(margin); // 사이드바와 메인 패널 사이 여백

    // 메인 패널 영역 설정 (우측)
    mainPanel->setBounds(mainArea);
}

void MainComponent::switchToPage(const juce::String& pageName)
{
    // 새 페이지를 먼저 생성
    std::unique_ptr<Page> newPage;
    
    try {
        if (pageName == "Home")
            newPage = std::make_unique<HomePage>();
        else if (pageName == "Upload")
            newPage = std::make_unique<UploadPage>();
        else if (pageName == "Library")
            newPage = std::make_unique<LibraryPage>();
        else if (pageName == "Courses")
            newPage = std::make_unique<CoursesPage>();
        else if (pageName == "Explore")
            newPage = std::make_unique<ExplorePage>();
    }
    catch (const std::exception& e) {
        DBG("Failed to create new page: " + juce::String(e.what()));
        return;
    }

    if (newPage == nullptr)
        return;

    // MessageManager를 통해 UI 업데이트를 안전하게 수행
    juce::MessageManager::callAsync([this, newPagePtr = newPage.release()]() mutable {
        std::unique_ptr<Page> newPage(newPagePtr);
        
        // 이전 페이지 제거 전에 새 페이지 준비
        if (newPage != nullptr)
        {
            newPage->setBounds(mainPanel != nullptr ? mainPanel->getBounds() : getLocalBounds());
            
            // 기존 페이지 제거
            if (mainPanel != nullptr)
            {
                removeChildComponent(mainPanel.get());
                mainPanel.reset();
            }

            // 새 페이지 추가
            addAndMakeVisible(*newPage);
            mainPanel = std::move(newPage);
            
            // 레이아웃 업데이트
            resized();
        }
    });
}