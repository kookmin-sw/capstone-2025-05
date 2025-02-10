#pragma once
#include <JuceHeader.h>
#include "../../UI/Styles/MapleColours.h"
#include "../../UI/Styles/MapleTypography.h"

class Page : public juce::Component
{
public:
    Page()
    {
        // 폰트 로드
        MapleTypography::loadCustomFonts();
    }
    
    ~Page() override = default;

    virtual juce::String getPageName() const = 0;

protected:
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds();

        // 둥근 모서리의 배경 그리기
        g.setColour(MapleColours::currentTheme.panel);
        g.fillRoundedRectangle(bounds.toFloat(), 10.0f);
    }

    // 안전한 텍스트 그리기 헬퍼 함수 추가
    void drawSafeText(juce::Graphics& g, const juce::String& text, const juce::Rectangle<int>& bounds)
    {
        static const juce::Font fallbackFont(24.0f);  // 기본 폰트
        
        auto customFont = MapleTypography::getPretendardBold(24.0f);
        if (customFont.getTypefaceName().isNotEmpty())
        {
            g.setFont(customFont);
        }
        else
        {
            g.setFont(fallbackFont);
        }
        
        g.drawText(text, bounds, juce::Justification::centredTop);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Page)
};