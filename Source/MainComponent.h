#pragma once

#include <JuceHeader.h>

/**
    MainComponent:
    - AudioAppComponent를 상속받아 오디오(마이크/기타) 입력을 받음
    - Timer를 사용해 GUI를 일정 주기로 갱신
    - paint()에서 파형을 그려주는 역할
*/
class MainComponent : public juce::AudioAppComponent,
	                  public juce::Button::Listener,
                      public juce::Timer	
{
public:
    MainComponent();
    ~MainComponent() override;

    void buttonClicked(juce::Button* button) override;  // 버튼 클릭 콜백 함수

    // AudioAppComponent에서 상속된 오디오 콜백
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;

    // Component에서 상속된 GUI 메서드
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    juce::TextButton settingsButton;
    std::shared_ptr<juce::DialogWindow> settingsWindow;

    static const int bufferSize = 1024;  // 버퍼 크기
    std::array<float, bufferSize> audioBuffer;  // 오디오 데이터 저장용 버퍼
    int bufferIndex = 0;  // 현재 버퍼 위치

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
