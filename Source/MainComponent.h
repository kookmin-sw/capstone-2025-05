#pragma once

#include <JuceHeader.h>
#include "Screen.h"
#include "StartScreenComponent.h"
#include "RecordingComponent.h"

class MainComponent : public juce::Component,
                     private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void showScreen(Screen screen);

private:
    void timerCallback() override;
    void startTransition(std::unique_ptr<juce::Component> newScreen);
    
    enum class TransitionDirection
    {
        Left,   // 왼쪽으로 이동 (뒤로가기)
        Right   // 오른쪽으로 이동 (앞으로가기)
    };
    
    void startSlideTransition(std::unique_ptr<juce::Component> newScreen, TransitionDirection direction);

    std::unique_ptr<juce::Component> currentScreen;
    std::unique_ptr<juce::Component> nextScreen;
    Screen currentScreenType;
    
    float transitionProgress = 0.0f;  // 0.0 ~ 1.0
    const float transitionSpeed = 0.055f;
    bool isTransitioning = false;
    TransitionDirection transitionDirection = TransitionDirection::Right;

    // 그림자 효과를 위한 상수
    const float shadowSize = 20.0f;
    const float maxElevation = 10.0f;
    
    // easing 함수들
    float easeInOutCubic(float t) const
    {
        return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
    }
    
    float getEasedProgress() const
    {
        return easeInOutCubic(transitionProgress);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
