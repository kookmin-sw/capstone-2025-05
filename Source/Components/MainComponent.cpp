#include "MainComponent.h"
#include "../Utils/Constants.h"
#include "../UI/Styles/MapleColours.h"

MainComponent::MainComponent()
{
    // 헤더 초기화
    addAndMakeVisible(headerComponent);

    // 사이드바 초기화
    addAndMakeVisible(sidebarComponent);

    // 메인 패널 초기화
    mainPanel = std::make_unique<HomePage>();
    addAndMakeVisible(mainPanel.get());

    // 푸터 초기화
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
    auto bounds = getLocalBounds().reduced(margin); // 윈도우 전체 마진

    // 헤더 영역 (상단)
    headerComponent.setBounds(bounds.removeFromTop(Constants::HEADER_HEIGHT));
    bounds.removeFromTop(margin); // 헤더와 메인 영역 사이 마진

    // 푸터 영역 (하단)
    auto footerBounds = bounds.removeFromBottom(Constants::FOOTER_HEIGHT);
    bounds.removeFromBottom(margin); // 메인 영역과 푸터 사이 마진
    footerComponent.setBounds(footerBounds);

    // 메인 영역 (사이드바 + 메인 패널)
    auto mainArea = bounds;

    // 사이드바 영역 (왼쪽)
    sidebarComponent.setBounds(mainArea.removeFromLeft(Constants::SIDEBAR_WIDTH));
    mainArea.removeFromLeft(margin); // 사이드바와 메인 패널 사이 마진

    // 메인 패널 영역 (오른쪽)
    mainPanel->setBounds(mainArea);
}