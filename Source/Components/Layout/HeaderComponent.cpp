#include "HeaderComponent.h"
#include "BinaryData.h"
#include "../../UI/Styles/MapleTypography.h"

HeaderComponent::HeaderComponent()
{
    // 폰트 로드
    MapleTypography::loadCustomFonts();

    // 로고 설정
    logoDrawable = juce::Drawable::createFromImageData(BinaryData::logo_svg, BinaryData::logo_svgSize);

    // 프로젝트 이름 설정
    addAndMakeVisible(projectNameLabel);
    projectNameLabel.setText(u8"MAPLE", juce::dontSendNotification);
    projectNameLabel.setFont(MapleTypography::getMontserratBold(36.0f));
    projectNameLabel.setColour(juce::Label::textColourId, MapleColours::currentTheme.logo);
    projectNameLabel.setJustificationType(juce::Justification::centredLeft);

    // 검색창 설정
    addAndMakeVisible(searchBox);
}

void HeaderComponent::paint(juce::Graphics &g)
{
    // 로고 그리기
    if (logoDrawable != nullptr)
    {
        auto bounds = getLocalBounds().reduced(20, 15);
        auto logoSection = bounds.removeFromLeft(40);
        logoDrawable->drawWithin(g, logoSection.toFloat().reduced(5),
                                 juce::RectanglePlacement::centred, 1.0f);
    }
}

void HeaderComponent::resized()
{
    auto bounds = getLocalBounds().reduced(20, 15);

    // 왼쪽 영역 (로고 + 프로젝트 이름)
    auto leftSection = bounds.removeFromLeft(200);

    // 로고 영역
    leftSection.removeFromLeft(40);
    leftSection.removeFromLeft(10); // 로고와 텍스트 사이 간격

    // 프로젝트 이름
    projectNameLabel.setBounds(leftSection);

    // 중앙 검색창 (양쪽에 여백을 주어 중앙에 위치)
    auto centerSection = bounds.reduced(bounds.getWidth() / 4, 0);
    // 검색창 높이를 55으로 설정하고 수직 중앙 정렬
    auto searchBoxBounds = centerSection.withHeight(55).withCentre(centerSection.getCentre());
    searchBoxBounds.setWidth(searchBoxBounds.getWidth() * 2 / 3);
    searchBox.setBounds(searchBoxBounds);
}