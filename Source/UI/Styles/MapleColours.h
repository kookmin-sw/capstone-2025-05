#pragma once
#include <JuceHeader.h>

namespace MapleColours
{
    struct Theme
    {
        juce::Colour background;
        juce::Colour foreground;
        juce::Colour buttonNormal;
        juce::Colour buttonHover;
        juce::Colour buttonPressed;
        juce::Colour buttonText;
        juce::Colour text;
        juce::Colour border;
    };

    namespace Dark
    {
        const Theme theme{
            juce::Colour(0xFF121212), // background
            juce::Colour(0xFF1E1E1E), // foreground
            juce::Colour(0xFF007AFF), // buttonNormal
            juce::Colour(0xFF3395FF), // buttonHover
            juce::Colour(0xFF0056B3), // buttonPressed
            juce::Colour(0xFFFFFFFF), // buttonText
            juce::Colour(0xFFE0E0E0), // text
            juce::Colour(0xFF2D2D2D)  // border
        };
    }

    namespace Light
    {
        const Theme theme{
            juce::Colour(0xFFFAFAFA), // background
            juce::Colour(0xFFFFFFFF), // foreground
            juce::Colour(0xFF007AFF), // buttonNormal
            juce::Colour(0xFF3395FF), // buttonHover
            juce::Colour(0xFF0056B3), // buttonPressed
            juce::Colour(0xFFFFFFFF), // buttonText
            juce::Colour(0xFF000000), // text
            juce::Colour(0xFFE0E0E0)  // border
        };
    }

    // 현재 활성화된 테마를 저장할 전역 변수
    extern Theme currentTheme;

    // 테마 전환 함수
    void setTheme(bool isDark);
}