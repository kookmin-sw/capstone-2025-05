#pragma once
#include <JuceHeader.h>

/**
 * Spotify 스타일의 커스텀 LookAndFeel 클래스
 * 앱 전체에 일관된 테마를 적용하기 위해 사용합니다.
 */
class MapleTheme : public juce::LookAndFeel_V4
{
public:
    MapleTheme()
    {
        // 기본 색상 테마 설정
        setColour(juce::ResizableWindow::backgroundColourId, getBackgroundColour());
        setColour(juce::TextButton::buttonColourId, getAccentColour());
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        
        setColour(juce::Slider::backgroundColourId, getBackgroundColour().brighter(0.1f));
        setColour(juce::Slider::thumbColourId, getAccentColour());
        setColour(juce::Slider::trackColourId, getAccentColour().withAlpha(0.5f));
        
        setColour(juce::Label::textColourId, juce::Colours::white);
        setColour(juce::TextEditor::backgroundColourId, getBackgroundColour().brighter(0.1f));
        setColour(juce::TextEditor::textColourId, juce::Colours::white);
        
        setColour(juce::ListBox::backgroundColourId, getBackgroundColour());
        setColour(juce::ListBox::outlineColourId, getBackgroundColour().brighter(0.2f));
        setColour(juce::ListBox::textColourId, juce::Colours::white);
    }
    
    // 버튼 드로잉 커스터마이징
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted, 
                             bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        auto cornerSize = 4.0f;
        
        auto baseColour = backgroundColour;
        
        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f);
            
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);
    }
    
    // TextButton 폰트 설정
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        return juce::Font("Segoe UI", buttonHeight * 0.5f, juce::Font::plain);
    }
    
    // 슬라이더 드로잉 커스터마이징
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style == juce::Slider::LinearBar || style == juce::Slider::LinearBarVertical)
        {
            auto baseColour = slider.findColour(juce::Slider::backgroundColourId);
            auto trackColour = slider.findColour(juce::Slider::trackColourId);
            
            g.setColour(baseColour);
            g.fillRoundedRectangle(x, y, width, height, 3.0f);
            
            g.setColour(trackColour);
            
            if (style == juce::Slider::LinearBar)
                g.fillRoundedRectangle(x, y, (int)sliderPos - x, height, 3.0f);
            else
                g.fillRoundedRectangle(x, (int)sliderPos, width, y + height - (int)sliderPos, 3.0f);
        }
        else
        {
            juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    }
    
    // 기본 Spotify 색상
    static juce::Colour getBackgroundColour() { return juce::Colour(0xff121212); } // 배경 검정
    static juce::Colour getAccentColour() { return juce::Colour(0xff1DB954); }     // Spotify 초록
    static juce::Colour getHighlightColour() { return juce::Colour(0xff1ED760); }  // 강조 초록
    static juce::Colour getSidebarColour() { return juce::Colour(0xff000000); }    // 사이드바 검정
    static juce::Colour getCardColour() { return juce::Colour(0xff282828); }       // 카드 배경
    static juce::Colour getTextColour() { return juce::Colours::white; }           // 기본 텍스트
    static juce::Colour getSubTextColour() { return juce::Colour(0xffB3B3B3); }    // 보조 텍스트
}; 