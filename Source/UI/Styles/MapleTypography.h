#pragma once
#include <JuceHeader.h>
#include "BinaryData.h"
namespace MapleTypography
{
    // 기본 폰트 크기
    const float SMALL = 14.0f;
    const float MEDIUM = 16.0f;
    const float LARGE = 24.0f;

    // 폰트 스타일
    const juce::Font button(MEDIUM, juce::Font::bold);

    struct CustomFonts
    {
        CustomFonts()
        {
            pretendardRegular = juce::Typeface::createSystemTypefaceFor(BinaryData::PretendardRegular_ttf, BinaryData::PretendardRegular_ttfSize);
            pretendardMedium = juce::Typeface::createSystemTypefaceFor(BinaryData::PretendardMedium_ttf, BinaryData::PretendardMedium_ttfSize);
            pretendardBold = juce::Typeface::createSystemTypefaceFor(BinaryData::PretendardBold_ttf, BinaryData::PretendardBold_ttfSize);
            montserratRegular = juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratRegular_ttf, BinaryData::MontserratRegular_ttfSize);
            montserratMedium = juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize);
            montserratBold = juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratBold_ttf, BinaryData::MontserratBold_ttfSize);
        }

        juce::Typeface::Ptr pretendardRegular, pretendardMedium, pretendardBold;
        juce::Typeface::Ptr montserratRegular, montserratMedium, montserratBold;
    };

    inline void loadCustomFonts()
    {
        static bool fontsLoaded = false;
        if (!fontsLoaded)
        {
            static CustomFonts fonts;

            // 기본 폰트 설정
            if (fonts.pretendardRegular != nullptr)
                juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(fonts.pretendardRegular);

            fontsLoaded = true;
        }
    }

    // 폰트 생성 함수들
    inline juce::Font getPretendardRegular(float size = MEDIUM)
    {
        static CustomFonts &fonts = []() -> CustomFonts &
        {
            static CustomFonts f;
            return f;
        }();
        return juce::Font(fonts.pretendardRegular).withHeight(size);
    }

    inline juce::Font getPretendardMedium(float size = MEDIUM)
    {
        static CustomFonts &fonts = []() -> CustomFonts &
        {
            static CustomFonts f;
            return f;
        }();
        return juce::Font(fonts.pretendardMedium).withHeight(size);
    }

    inline juce::Font getPretendardBold(float size = LARGE)
    {
        static CustomFonts &fonts = []() -> CustomFonts &
        {
            static CustomFonts f;
            return f;
        }();
        return juce::Font(fonts.pretendardBold).withHeight(size);
    }

    inline juce::Font getMontserratRegular(float size = MEDIUM)
    {
        static CustomFonts &fonts = []() -> CustomFonts &
        {
            static CustomFonts f;
            return f;
        }();
        return juce::Font(fonts.montserratRegular).withHeight(size);
    }

    inline juce::Font getMontserratMedium(float size = MEDIUM)
    {
        static CustomFonts &fonts = []() -> CustomFonts &
        {
            static CustomFonts f;
            return f;
        }();
        return juce::Font(fonts.montserratMedium).withHeight(size);
    }

    inline juce::Font getMontserratBold(float size = LARGE)
    {
        static CustomFonts &fonts = []() -> CustomFonts &
        {
            static CustomFonts f;
            return f;
        }();
        return juce::Font(fonts.montserratBold).withHeight(size);
    }
}