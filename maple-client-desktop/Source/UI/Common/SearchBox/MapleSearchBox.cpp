#include "MapleSearchBox.h"

MapleSearchBox::MapleSearchBox()
{
    setMultiLine(false);
    setReturnKeyStartsNewLine(false);
    setScrollbarsShown(false);
    setCaretVisible(true);
    setPopupMenuEnabled(true);

    setTextToShowWhenEmpty(juce::String::fromUTF8(u8"어떤 곡을 연습하고 싶으신가요?"), MapleColours::currentTheme.text);
    setFont(MapleTypography::getPretendardMedium(24.0f));

    // 색상 설정
    setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::TextEditor::textColourId, juce::Colours::white);
    setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::TextEditor::highlightColourId, juce::Colours::white.withAlpha(0.3f));
    setColour(juce::CaretComponent::caretColourId, MapleColours::currentTheme.text);

    // 텍스트 수직 중앙 정렬
    setJustification(juce::Justification::centredLeft);
    setIndents(50, 0);

    applyFontToAllText(MapleTypography::getPretendardMedium(22.0f));
    setLookAndFeel(&customLookAndFeel);

    // 돋보기 아이콘 로드
    searchIcon = juce::Drawable::createFromImageData(BinaryData::search_svg, BinaryData::search_svgSize);
}

MapleSearchBox::~MapleSearchBox()
{
    setLookAndFeel(nullptr);
}

void MapleSearchBox::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    auto cornerSize = bounds.getHeight() * 0.5f;

    // 배경 그리기
    g.setColour(MapleColours::currentTheme.panel);
    g.fillRoundedRectangle(bounds, cornerSize);

    // 포커스 효과
    if (hasKeyboardFocus(true))
    {
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }

    // 돋보기 아이콘 그리기
    if (searchIcon != nullptr)
    {
        auto iconBounds = bounds.removeFromLeft(50.0f).reduced(15.0f);
        searchIcon->drawWithin(g, iconBounds,
                               juce::RectanglePlacement::centred, 1.0f);
    }

    TextEditor::paint(g);
}

void MapleSearchBox::CustomLookAndFeel::drawTextEditorOutline(juce::Graphics &, int, int, juce::TextEditor &)
{
    // 아무것도 그리지 않음 (기본 테두리 제거)
}