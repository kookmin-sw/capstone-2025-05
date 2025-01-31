#include "HomePage.h"
#include "../../UI/Styles/MapleColours.h"
#include "../../UI/Styles/MapleTypography.h"

HomePage::HomePage()
{
    createProjectBtn = std::make_unique<MapleButton>(juce::String::fromUTF8(u8"+ 프로젝트 시작"));
    createProjectBtn->setColour(MapleButton::backgroundColourId, juce::Colours::black);
    createProjectBtn->setColour(MapleButton::textColourId, MapleColours::currentTheme.text);
    addAndMakeVisible(createProjectBtn.get());
}

HomePage::~HomePage()
{
}

void HomePage::paint(juce::Graphics &g)
{
    // 부모 클래스의 paint 호출 (배경 그리기)
    Page::paint(g);

    auto bounds = getLocalBounds().reduced(20);

    // 프로젝트 시작 버튼 영역
    auto projectButtonArea = bounds.removeFromTop(200);
    createProjectBtn->setFont(MapleTypography::getPretendardBold(24.0f));

    bounds.removeFromTop(40); // 프로젝트 버튼과 섹션 사이 간격

    // Recently 섹션
    auto recentlySection = bounds.removeFromTop(240);
    drawSection(g, juce::String::fromUTF8(u8"RECENTLY"), recentlySection);
    bounds.removeFromTop(40); // 섹션 간 간격

    // Recommended 섹션
    auto recommendedSection = bounds.removeFromTop(240);
    drawSection(g, juce::String::fromUTF8(u8"RECOMMENDED"), recommendedSection);
    bounds.removeFromTop(40); // 섹션 간 간격

    // Practice Courses 섹션
    auto practiceSection = bounds;
    drawSection(g, juce::String::fromUTF8(u8"PRACTICE COURSES"), practiceSection, false);
}

void HomePage::drawSection(juce::Graphics &g, const juce::String &title,
                           juce::Rectangle<int> bounds, bool showMoreButton)
{
    // 섹션 헤더 그리기
    auto headerBounds = bounds.removeFromTop(40);
    drawSectionHeader(g, title, headerBounds, showMoreButton);

    // 섹션 컨텐츠 영역 (회색 박스들)
    auto contentBounds = bounds;

    if (title == juce::String::fromUTF8(u8"PRACTICE COURSES"))
    {
        // Practice Courses는 더 큰 박스 3개
        const int boxWidth = (contentBounds.getWidth() - 40) / 3;
        const int boxHeight = 160;

        for (int i = 0; i < 3; ++i)
        {
            auto boxBounds = contentBounds.removeFromLeft(boxWidth);
            g.setColour(juce::Colours::black);
            g.fillRoundedRectangle(boxBounds.reduced(10).toFloat(), 8.0f);

            // 박스 내부에 텍스트 추가 (임시)
            g.setColour(MapleColours::currentTheme.text);
            g.setFont(MapleTypography::getPretendardMedium(16.0f));
            g.drawText(juce::String::fromUTF8(u8"연습 코스 ") + juce::String(i + 1),
                       boxBounds.reduced(20),
                       juce::Justification::centredTop);

            if (i < 2)
                contentBounds.removeFromLeft(20); // 간격
        }
    }
    else
    {
        // Recently와 Recommended는 작은 박스 6개
        const int boxWidth = (contentBounds.getWidth() - 100) / 6;
        const int boxHeight = 160;

        for (int i = 0; i < 6; ++i)
        {
            auto boxBounds = contentBounds.removeFromLeft(boxWidth);
            g.setColour(juce::Colours::black);
            g.fillRoundedRectangle(boxBounds.reduced(10).toFloat(), 8.0f);

            // 박스 내부에 텍스트 추가 (임시)
            g.setColour(MapleColours::currentTheme.text);
            g.setFont(MapleTypography::getPretendardMedium(14.0f));
            g.drawText(title == juce::String::fromUTF8(u8"RECENTLY") ? juce::String::fromUTF8(u8"최근 항목 ") + juce::String(i + 1) : juce::String::fromUTF8(u8"추천 항목 ") + juce::String(i + 1),
                       boxBounds.reduced(15),
                       juce::Justification::centredTop);

            if (i < 5)
                contentBounds.removeFromLeft(20); // 간격
        }
    }
}

void HomePage::drawSectionHeader(juce::Graphics &g, const juce::String &title,
                                 juce::Rectangle<int> bounds, bool showMoreButton)
{
    // 섹션 제목
    g.setFont(MapleTypography::getMontserratBold(24.0f));
    g.setColour(MapleColours::currentTheme.text);
    g.drawText(title, bounds.removeFromLeft(bounds.getWidth() - 100),
               juce::Justification::centredLeft);

    // "모두 보기" 버튼
    if (showMoreButton)
    {
        g.setFont(MapleTypography::getPretendardMedium(14.0f));
        g.setColour(MapleColours::currentTheme.text.withAlpha(0.7f));
        g.drawText(juce::String::fromUTF8(u8"모두 보기"), bounds.removeFromRight(100),
                   juce::Justification::centredRight);
    }
}

void HomePage::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    // 프로젝트 시작 버튼 크기 및 위치 설정
    auto projectButtonArea = bounds.removeFromTop(200);
    createProjectBtn->setBounds(projectButtonArea.reduced(10).withHeight(160));
}