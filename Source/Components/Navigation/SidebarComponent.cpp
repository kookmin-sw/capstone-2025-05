#include "SidebarComponent.h"
#include "../../UI/Styles/MapleColours.h"
#include "../../UI/Styles/MapleTypography.h"
#include "BinaryData.h"

SidebarComponent::SidebarComponent()
{
    // 상단 메뉴 아이템 생성
    createMenuItem(menuItems, BinaryData::home_svg, BinaryData::home_svgSize, "Home");
    createMenuItem(menuItems, BinaryData::upload_svg, BinaryData::upload_svgSize, "Upload");
    createMenuItem(menuItems, BinaryData::library_svg, BinaryData::library_svgSize, "Library");
    createMenuItem(menuItems, BinaryData::courses_svg, BinaryData::courses_svgSize, "Courses");
    createMenuItem(menuItems, BinaryData::explore_svg, BinaryData::explore_svgSize, "Explore");

    // 하단 메뉴 아이템 생성
    createMenuItem(bottomMenuItems, BinaryData::settings_svg, BinaryData::settings_svgSize, "Settings");
    createMenuItem(bottomMenuItems, BinaryData::profile_svg, BinaryData::profile_svgSize, "Profile");
    createMenuItem(bottomMenuItems, BinaryData::logout_svg, BinaryData::logout_svgSize, "Logout");

    startTimer(16);
}

SidebarComponent::~SidebarComponent()
{
}

void SidebarComponent::createMenuItem(std::vector<MenuItem> &items, const char *iconData, size_t iconSize,
                                      const juce::String &text, std::function<void()> onClick)
{
    MenuItem item;
    item.icon = juce::Drawable::createFromImageData(iconData, iconSize);
    item.text = text;
    item.onClick = onClick;
    items.push_back(std::move(item));
}

void SidebarComponent::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();

    // 배경 그리기
    g.setColour(MapleColours::currentTheme.panel);
    g.fillRoundedRectangle(bounds.toFloat(), 10.0f);

    // 상단 메뉴 아이템 그리기
    auto itemBounds = bounds.reduced(10, 20);
    const int itemHeight = 50;
    const int spacing = 15;

    for (size_t i = 0; i < menuItems.size(); ++i)
    {
        auto itemRect = itemBounds.removeFromTop(itemHeight);
        drawMenuItem(g, menuItems[i], itemRect, i == hoveredIndex);
        itemBounds.removeFromTop(spacing);
    }

    // 하단 메뉴 아이템 그리기
    itemBounds = bounds.reduced(10, 20).withY(bounds.getHeight() - (bottomMenuItems.size() * (itemHeight + spacing)));

    for (size_t i = 0; i < bottomMenuItems.size(); ++i)
    {
        auto itemRect = itemBounds.removeFromTop(itemHeight);
        drawMenuItem(g, bottomMenuItems[i], itemRect, i == hoveredBottomIndex);
        itemBounds.removeFromTop(spacing);
    }
}

void SidebarComponent::drawMenuItem(juce::Graphics &g, const MenuItem &item, const juce::Rectangle<int> &bounds, bool isHovered)
{
    // 호버 효과
    if (item.alpha > 0.0f)
    {
        g.setColour(MapleColours::currentTheme.buttonHover.withAlpha(0.1f * item.alpha));
        g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    }

    // 아이콘 그리기
    if (item.icon != nullptr)
    {
        auto iconBounds = bounds.reduced(12).withWidth(24); // 아이콘 크기를 28x28로 증가
        item.icon->drawWithin(g, iconBounds.toFloat(),
                              juce::RectanglePlacement::centred,
                              0.8f + (0.2f * item.alpha));
    }

    // 텍스트 그리기
    g.setFont(MapleTypography::getMontserratSemiBold(26.0f)); // 폰트 크기를 20으로 증가
    g.setColour(MapleColours::currentTheme.text.withAlpha(0.8f + (0.2f * item.alpha)));

    // 텍스트 영역 계산
    auto textBounds = bounds.withTrimmedLeft(55); // 아이콘과 텍스트 사이 간격 조정

    // 텍스트를 수직 중앙 정렬하기 위한 계산
    int textHeight = g.getCurrentFont().getHeight();
    int verticalOffset = (bounds.getHeight() - textHeight) / 2;

    // 텍스트 그리기 (수직 중앙 정렬)
    g.drawText(item.text,
               textBounds.reduced(10, verticalOffset),
               juce::Justification::centredLeft,
               true);
}

void SidebarComponent::resized()
{
    // 크기가 변경될 때 필요한 작업이 없으면 비워둡니다
}

void SidebarComponent::mouseMove(const juce::MouseEvent &event)
{
    auto bounds = getLocalBounds().reduced(10, 20);
    const int itemHeight = 50;
    const int spacing = 15;

    bool wasHovered = (hoveredIndex != -1 || hoveredBottomIndex != -1);
    bool isNowHovered = false;

    // 상단 메뉴 아이템 검사
    auto itemBounds = bounds;
    for (int i = 0; i < menuItems.size(); ++i)
    {
        if (itemBounds.removeFromTop(itemHeight).contains(event.getPosition()))
        {
            hoveredIndex = i;
            hoveredBottomIndex = -1;
            isNowHovered = true;
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
            repaint();
            return;
        }
        itemBounds.removeFromTop(spacing);
    }

    // 하단 메뉴 아이템 검사
    itemBounds = bounds.withY(bounds.getHeight() - (bottomMenuItems.size() * (itemHeight + spacing)));
    for (int i = 0; i < bottomMenuItems.size(); ++i)
    {
        if (itemBounds.removeFromTop(itemHeight).contains(event.getPosition()))
        {
            hoveredIndex = -1;
            hoveredBottomIndex = i;
            isNowHovered = true;
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
            repaint();
            return;
        }
        itemBounds.removeFromTop(spacing);
    }

    // 아무 항목도 호버되지 않은 경우
    if (wasHovered || !isNowHovered)
    {
        hoveredIndex = -1;
        hoveredBottomIndex = -1;
        setMouseCursor(juce::MouseCursor::NormalCursor);
        repaint();
    }
}

void SidebarComponent::mouseExit(const juce::MouseEvent &)
{
    hoveredIndex = -1;
    hoveredBottomIndex = -1;
    repaint();
}

void SidebarComponent::timerCallback()
{
    bool needsRepaint = false;
    const float animationSpeed = 0.2f; // 애니메이션 속도 조절

    // 상단 메뉴 아이템 애니메이션
    for (size_t i = 0; i < menuItems.size(); ++i)
    {
        float targetAlpha = (i == hoveredIndex) ? 1.0f : 0.0f;
        float &currentAlpha = menuItems[i].alpha;

        if (currentAlpha != targetAlpha)
        {
            currentAlpha += (targetAlpha - currentAlpha) * animationSpeed;
            if (std::abs(currentAlpha - targetAlpha) < 0.01f)
                currentAlpha = targetAlpha;
            needsRepaint = true;
        }
    }

    // 하단 메뉴 아이템 애니메이션
    for (size_t i = 0; i < bottomMenuItems.size(); ++i)
    {
        float targetAlpha = (i == hoveredBottomIndex) ? 1.0f : 0.0f;
        float &currentAlpha = bottomMenuItems[i].alpha;

        if (currentAlpha != targetAlpha)
        {
            currentAlpha += (targetAlpha - currentAlpha) * animationSpeed;
            if (std::abs(currentAlpha - targetAlpha) < 0.01f)
                currentAlpha = targetAlpha;
            needsRepaint = true;
        }
    }

    if (needsRepaint)
        repaint();
}