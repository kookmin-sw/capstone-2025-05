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
    projectNameLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF2ECC71)); // 녹색 로고
    projectNameLabel.setJustificationType(juce::Justification::centredLeft);

    // 검색창 설정
    addAndMakeVisible(searchBox);
    searchBox.setTextToShowWhenEmpty(juce::String::fromUTF8(u8"어떤 곡을 연습하고 싶으신가요?"), juce::Colours::grey);
    searchBox.setFont(MapleTypography::getPretendardMedium(18.0f));
    searchBox.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    searchBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    searchBox.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    searchBox.setJustification(juce::Justification::centredLeft);
}

void HeaderComponent::paint(juce::Graphics &g)
{
    g.setColour(MapleColours::currentTheme.panel);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);

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
    searchBox.setBounds(centerSection);
}