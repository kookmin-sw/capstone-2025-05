#include "MainComponent.h"
#include "../Utils/Constants.h"
#include "../UI/Styles/MapleColours.h"

MainComponent::MainComponent()
{
    // 헤더 컴포넌트 초기화 및 표시
    addAndMakeVisible(headerComponent);

    // 사이드바 컴포넌트 초기화 및 표시
    addAndMakeVisible(sidebarComponent);

    // 메인 패널 컴포넌트 초기화 및 표시 (홈페이지)
    mainPanel = std::make_unique<HomePage>();
    addAndMakeVisible(mainPanel.get());

    // 푸터 컴포넌트 초기화 및 표시
    addAndMakeVisible(footerComponent);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(MapleColours::currentTheme.background);
}

void MainComponent::resized()
{
    const int margin = 10;
    auto bounds = getLocalBounds().reduced(margin); // 전체 컴포넌트에 마진 적용

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