#pragma once
#include <JuceHeader.h>
#include "MapleTheme.h"

/**
 * 세련된 스타일의 Play 버튼을 위한 커스텀 LookAndFeel 클래스
 */
class MaplePlayButton : public juce::LookAndFeel_V4
{
public:
    MaplePlayButton()
    {
        // 버튼 색상 설정
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1DB954));         // Spotify 녹색과 유사한 색상
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF1ED760));       // 활성화 시 더 밝은 녹색
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                              const juce::Colour& backgroundColour, 
                              bool shouldDrawButtonAsHighlighted, 
                              bool shouldDrawButtonAsDown) override
    {
        // 그림자 효과
        float shadowAlpha = button.isEnabled() ? 0.4f : 0.2f;
        
        // 재생 중일 때 (버튼이 눌려있을 때) 다른 형태의 버튼
        juce::Colour baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                                                .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);
        
        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);
        
        const float width  = button.getWidth() - 1.0f;
        const float height = button.getHeight() - 1.0f;
        
        // 둥근 사각형 배경
        g.setGradientFill(juce::ColourGradient(baseColour, 0.0f, 0.0f,
                                             baseColour.darker(0.1f), 0.0f, height,
                                             false));
        
        // 사각형 버튼 대신 원형 버튼으로 변경
        const float diameter = juce::jmin(width, height);
        const float radius = diameter / 2.0f;
        const float centerX = width / 2.0f;
        const float centerY = height / 2.0f;
        
        // 그림자
        g.setColour(juce::Colours::black.withAlpha(shadowAlpha));
        g.fillEllipse(centerX - radius + 1.5f, centerY - radius + 1.5f, diameter, diameter);
        
        // 메인 버튼
        g.setColour(baseColour);
        g.fillEllipse(centerX - radius, centerY - radius, diameter, diameter);
        
        // 테두리
        g.setColour(button.findColour(button.getToggleState() ? 
                               juce::TextButton::textColourOnId : 
                               juce::TextButton::textColourOffId).withMultipliedAlpha(0.4f));
        g.drawEllipse(centerX - radius, centerY - radius, diameter, diameter, 1.0f);
    }
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, 
                         bool shouldDrawButtonAsHighlighted, 
                         bool shouldDrawButtonAsDown) override
    {
        // 텍스트 대신 재생 아이콘 그리기
        const float width = button.getWidth();
        const float height = button.getHeight();
        
        // 버튼의 중앙에 재생/일시정지 아이콘 그리기
        g.setColour(button.findColour(button.getToggleState() ? 
                          juce::TextButton::textColourOnId : 
                          juce::TextButton::textColourOffId));
        
        if (button.getToggleState()) {
            // 일시정지 아이콘 (두 개의 세로 막대)
            const float pauseBarWidth = width * 0.04f;  // 더 얇게 (0.08f → 0.06f)
            const float pauseBarHeight = height * 0.45f; // 약간 더 길게 (0.45f → 0.48f)
            const float pauseBarGap = width * 0.05f;     // 간격 좁히기 (0.12f → 0.08f)
            const float yOffset = (height - pauseBarHeight) * 0.5f;
            const float xOffset = (width - (pauseBarWidth * 2 + pauseBarGap)) * 0.5f;
            
            g.fillRect(xOffset, yOffset, pauseBarWidth, pauseBarHeight);
            g.fillRect(xOffset + pauseBarWidth + pauseBarGap, yOffset, pauseBarWidth, pauseBarHeight);
        } else {
            // 재생 아이콘 (삼각형)
            juce::Path path;
            const float triangleSize = juce::jmin(width, height) * 0.35f;
            const float xCenter = width * 0.5f + triangleSize * 0.1f; // 약간 오른쪽으로 이동
            const float yCenter = height * 0.5f;
            
            path.addTriangle(xCenter - triangleSize * 0.5f, yCenter - triangleSize * 0.5f,
                           xCenter - triangleSize * 0.5f, yCenter + triangleSize * 0.5f,
                           xCenter + triangleSize * 0.5f, yCenter);
            
            g.fillPath(path);
        }
    }
};