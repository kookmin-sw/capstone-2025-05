#pragma once
#include <JuceHeader.h>
#include "Model/TabPlayer.h"

class ScoreComponent : public juce::Component,
                      public juce::Timer
{
public:
    ScoreComponent(TabPlayer& player);
    ~ScoreComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    bool keyPressed(const juce::KeyPress& key) override;
    
    void startPlayback();
    void stopPlayback();
    
    // 악보 업데이트 메서드 추가
    void updateScore();

private:
    void timerCallback() override; // 재생 위치 업데이트를 위해 주기적 호출

    TabPlayer& tabPlayer; // TabPlayer 참조
    
    // 스코어 콘텐츠 컴포넌트 - 악보 내용을 실제로 그림
    class ScoreContentComponent : public juce::Component
    {
    public:
        ScoreContentComponent(ScoreComponent& parent, TabPlayer& player) 
            : owner(parent), tabPlayer(player) {}
        
        void paint(juce::Graphics& g) override;
        
    private:
        ScoreComponent& owner;
        TabPlayer& tabPlayer;
    };
    
    ScoreContentComponent scoreContent { *this, tabPlayer };
    juce::Viewport viewport; // 스크롤 가능한 뷰

    // 렌더링 관련 상수
    const float stringSpacing = 20.0f;
    const float noteSpacing = 30.0f;
    const float xOffset = 20.0f;
    const float yOffset = 50.0f;

    friend class ScoreContentComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScoreComponent)
};