#include "MapleButton.h"
#include "../../Styles/MapleColours.h"
#include "../../Styles/MapleTypography.h"

MapleButton::MapleButton(const juce::String &buttonText) : text(buttonText)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void MapleButton::setText(const juce::String &newText)
{
    text = newText;
    repaint();
}

void MapleButton::setOnClick(std::function<void()> callback)
{
    onClick = callback;
}

void MapleButton::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();

    // 배경 그리기
    g.setColour(isMouseDown   ? MapleColours::Dark::theme.buttonPressed
                : isMouseOver ? MapleColours::Dark::theme.buttonHover
                              : MapleColours::Dark::theme.buttonNormal);
    g.fillRoundedRectangle(bounds, 8.0f);

    // 텍스트 그리기
    g.setFont(MapleTypography::button);
    g.setColour(MapleColours::Dark::theme.buttonText);
    g.drawText(text, bounds, juce::Justification::centred);
}

void MapleButton::resized()
{
    // 크기가 변경될 때 필요한 작업이 없으면 비워둡니다
}

void MapleButton::mouseEnter(const juce::MouseEvent &)
{
    isMouseOver = true;
    repaint();
}

void MapleButton::mouseExit(const juce::MouseEvent &)
{
    isMouseOver = false;
    repaint();
}

void MapleButton::mouseDown(const juce::MouseEvent &)
{
    isMouseDown = true;
    repaint();
}

void MapleButton::mouseUp(const juce::MouseEvent &)
{
    isMouseDown = false;
    repaint();

    if (onClick)
        onClick();
}

// ... 나머지 구현