#include "CustomButton.h"
#include "../../../UI/Styles/Colours.h"
#include "../../../UI/Styles/Typography.h"

CustomButton::CustomButton(const juce::String &buttonText) : text(buttonText)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void CustomButton::setText(const juce::String &newText)
{
    text = newText;
    repaint();
}

void CustomButton::setOnClick(std::function<void()> callback)
{
    onClick = callback;
}

void CustomButton::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds().toFloat();

    // 배경 그리기
    g.setColour(isMouseDown   ? Colours::buttonPressed
                : isMouseOver ? Colours::buttonHover
                              : Colours::buttonNormal);
    g.fillRoundedRectangle(bounds, 8.0f);

    // 텍스트 그리기
    g.setFont(Typography::button);
    g.setColour(Colours::buttonText);
    g.drawText(text, bounds, juce::Justification::centred);
}

void CustomButton::resized()
{
    // 크기가 변경될 때 필요한 작업이 없으면 비워둡니다
}

void CustomButton::mouseEnter(const juce::MouseEvent &)
{
    isMouseOver = true;
    repaint();
}

void CustomButton::mouseExit(const juce::MouseEvent &)
{
    isMouseOver = false;
    repaint();
}

void CustomButton::mouseDown(const juce::MouseEvent &)
{
    isMouseDown = true;
    repaint();
}

void CustomButton::mouseUp(const juce::MouseEvent &)
{
    isMouseDown = false;
    repaint();

    if (onClick)
        onClick();
}

// ... 나머지 구현