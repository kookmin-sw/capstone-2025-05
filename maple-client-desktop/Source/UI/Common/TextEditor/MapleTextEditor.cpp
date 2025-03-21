#include "MapleTextEditor.h"

MapleTextEditor::MapleTextEditor()
{
    // 기본 스타일 설정
    setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
    setBorder(juce::BorderSize<int>(0, 0, 0, 0));
    setJustification(juce::Justification::centredLeft);
}

void MapleTextEditor::focusGained(juce::Component::FocusChangeType cause)
{
    TextEditor::focusGained(cause);
    startAnimation();
}

void MapleTextEditor::focusLost(juce::Component::FocusChangeType cause)
{
    TextEditor::focusLost(cause);
    resetAnimation();
}

void MapleTextEditor::paint(juce::Graphics& g)
{
    TextEditor::paint(g);

    auto bounds = getLocalBounds();
    const float y = bounds.getBottom() - 1.0f;
    
    // 기본 밑줄
    g.setColour(MapleColours::currentTheme.border);
    g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);

    if (isAnimating)
    {
        // 애니메이션 중인 밑줄
        const float center = bounds.getCentreX();
        const float easedProgress = easeOutCubic(animationProgress);
        const float width = bounds.getWidth() * easedProgress;
        g.setColour(MapleColours::currentTheme.buttonNormal);
        g.drawLine(center - width/2, y, center + width/2, y, 2.0f);
    }
}

void MapleTextEditor::setAnimationSpeed(float speed)
{
    animationSpeed = speed;
}

float MapleTextEditor::easeOutCubic(float x)
{
    return 1.0f - pow(1.0f - x, 3.0f);
}

void MapleTextEditor::startAnimation()
{
    isAnimating = true;
    animationProgress = 0.0f;
    startTime = juce::Time::getMillisecondCounterHiRes();
    startTimerHz(60);
}

void MapleTextEditor::resetAnimation()
{
    isAnimating = false;
    animationProgress = 0.0f;
    stopTimer();
    repaint();
}

void MapleTextEditor::timerCallback()
{
    const double currentTime = juce::Time::getMillisecondCounterHiRes();
    const double elapsedTime = currentTime - startTime;
    const double duration = 300.0; // 애니메이션 지속 시간 (밀리초)

    animationProgress = static_cast<float>(juce::jmin(elapsedTime / duration, 1.0));

    if (animationProgress >= 1.0f)
    {
        animationProgress = 1.0f;
        stopTimer();
    }

    repaint();
} 