#pragma once
#include <JuceHeader.h>
#include "../../UI/Styles/MapleColours.h"
#include "../../UI/Common/SearchBox/MapleSearchBox.h"

class HeaderComponent : public juce::Component
{
public:
    HeaderComponent();
    ~HeaderComponent() override = default;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    // 로고 이미지
    std::unique_ptr<juce::Drawable> logoDrawable;

    // 프로젝트 이름
    juce::Label projectNameLabel;

    // 검색창
    MapleSearchBox searchBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderComponent)
};