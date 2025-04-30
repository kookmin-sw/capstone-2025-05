#pragma once
#include <JuceHeader.h>

/**
 * 세련된 스타일의 분석 버튼을 위한 커스텀 LookAndFeel 클래스
 */
class MapleAnalyzeButton : public juce::LookAndFeel_V4
{
public:
    MapleAnalyzeButton()
    {
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xff5c5cff));
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff7c7cff));
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted, 
                             bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto centre = bounds.getCentre();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        
        // 기본 배경 (원형)
        juce::Colour baseColour = backgroundColour;
        
        if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f);
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.brighter(0.2f);
        
        g.setColour(baseColour);
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);
        
        // 분석 아이콘 그리기
        g.setColour(juce::Colours::white);
        
        if (isAnalyzing)
        {
            // 분석 중 - 회전하는 로딩 아이콘
            static float rotation = 0.0f;
            rotation += 0.05f;
            
            g.saveState();
            g.addTransform(juce::AffineTransform::rotation(rotation, centre.x, centre.y));
            
            for (int i = 0; i < 8; ++i)
            {
                float angle = i * juce::MathConstants<float>::pi * 0.25f;
                float alpha = 0.3f + 0.7f * std::fmod(i * 0.125f + std::fmod(rotation, 1.0f), 1.0f);
                
                g.setColour(juce::Colours::white.withAlpha(alpha));
                
                float x = centre.x + radius * 0.6f * std::cos(angle);
                float y = centre.y + radius * 0.6f * std::sin(angle);
                
                g.fillEllipse(x - radius * 0.1f, y - radius * 0.1f, 
                             radius * 0.2f, radius * 0.2f);
            }
            
            g.restoreState();
        }
        else
        {
            // 분석 대기 - 막대 그래프 아이콘
            float barWidth = radius * 0.15f;
            float barSpacing = radius * 0.1f;
            float barBottom = centre.y + radius * 0.4f;
            float maxBarHeight = radius * 0.8f;
            
            for (int i = 0; i < 3; ++i)
            {
                float barHeight = maxBarHeight * (0.4f + (i % 3) * 0.3f);
                float x = centre.x - barWidth * 1.5f - barSpacing + i * (barWidth + barSpacing);
                
                g.fillRect(x, barBottom - barHeight, barWidth, barHeight);
            }
        }
    }
    
    void setAnalyzing(bool analyzing)
    {
        isAnalyzing = analyzing;
    }
    
private:
    bool isAnalyzing = false;
};