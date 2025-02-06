#include "MainComponent.h"
#include "../Utils/Constants.h"
#include "../UI/Styles/MapleColours.h"
#include "Pages/HomePage.h"

MainComponent::MainComponent()
{
    // 메인 UI 구성 요소 초기화
    addAndMakeVisible(headerComponent);
    addAndMakeVisible(sidebarComponent);
    addAndMakeVisible(footerComponent);

    mainPanel = std::make_unique<HomePage>();
    addAndMakeVisible(mainPanel.get());
}

MainComponent::~MainComponent()
{
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