#pragma once
#include <JuceHeader.h>

/**
 * 세련된 스타일의 녹음 버튼을 위한 커스텀 LookAndFeel 클래스
 */
class MapleRecordButton : public juce::LookAndFeel_V4
{
public:
    MapleRecordButton()
    {
        // 버튼 색상 설정
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFE74C3C));         // 녹음 버튼 빨간색
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFFe57373));       // 활성화 시 더 밝은 빨간색
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        pulseAlpha = 1.0f;
        isRecordingActive = false;
    }
    
    void setRecording(bool isRecording)
    {
        isRecordingActive = isRecording;
        pulseAlpha = 1.0f;
    }
    
    void updatePulse()
    {
        if (isRecordingActive)
        {
            // 펄싱 효과를 위한 알파값 업데이트
            pulseValue = (pulseValue + 0.05f);
            if (pulseValue > 1.0f)
                pulseValue = 0.0f;
                
            pulseAlpha = 0.5f + 0.5f * std::sin(pulseValue * juce::MathConstants<float>::twoPi);
        }
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                              const juce::Colour& backgroundColour, 
                              bool shouldDrawButtonAsHighlighted, 
                              bool shouldDrawButtonAsDown) override
    {
        // 그림자 효과
        float shadowAlpha = button.isEnabled() ? 0.4f : 0.2f;
        
        // 색상 설정
        juce::Colour baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                                                .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);
        
        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);
        
        // 녹음 중일 경우 색상 변화 (펄싱 효과)
        if (isRecordingActive)
            baseColour = baseColour.withAlpha(pulseAlpha);
            
        const float width  = button.getWidth() - 1.0f;
        const float height = button.getHeight() - 1.0f;
        
        // 원형 버튼 그리기
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
        // 텍스트 대신 녹음 아이콘 (원 또는 사각형) 그리기
        const float width = button.getWidth();
        const float height = button.getHeight();
        
        // 버튼의 중앙에 아이콘 그리기
        g.setColour(button.findColour(button.getToggleState() ? 
                          juce::TextButton::textColourOnId : 
                          juce::TextButton::textColourOffId));
        
        // 녹음 중일 때와 아닐 때의 아이콘 다르게 표시
        if (isRecordingActive) {
            // 녹음 중지 아이콘 (사각형)
            float size = juce::jmin(width, height) * 0.3f;
            g.fillRect(width * 0.5f - size * 0.5f, 
                      height * 0.5f - size * 0.5f, 
                      size, size);
        } else {
            // 녹음 시작 아이콘 (원)
            float size = juce::jmin(width, height) * 0.3f;
            g.fillEllipse(width * 0.5f - size * 0.5f, 
                         height * 0.5f - size * 0.5f, 
                         size, size);
        }
    }
    
private:
    float pulseAlpha = 1.0f;
    float pulseValue = 0.0f;
    bool isRecordingActive = false;
};