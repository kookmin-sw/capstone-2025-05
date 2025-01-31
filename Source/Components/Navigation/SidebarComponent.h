#pragma once
#include <JuceHeader.h>
#include "../../UI/Common/Button/MapleButton.h"

class SidebarComponent : public juce::Component,
                         private juce::Timer // Timer 추가
{
public:
    SidebarComponent();
    ~SidebarComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    // 마우스 이벤트 처리를 위한 함수들 추가
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void timerCallback() override; // Timer 콜백 추가

private:
    struct MenuItem
    {
        std::unique_ptr<juce::Drawable> icon;
        juce::String text;
        std::function<void()> onClick;
        float alpha = 0.0f; // 호버 애니메이션을 위한 알파값
    };

    std::vector<MenuItem> menuItems;
    std::vector<MenuItem> bottomMenuItems;

    void createMenuItem(std::vector<MenuItem> &items, const char *iconData, size_t iconSize,
                        const juce::String &text, std::function<void()> onClick = nullptr);
    void drawMenuItem(juce::Graphics &g, const MenuItem &item, const juce::Rectangle<int> &bounds, bool isHovered);

    int hoveredIndex = -1;
    int hoveredBottomIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
};