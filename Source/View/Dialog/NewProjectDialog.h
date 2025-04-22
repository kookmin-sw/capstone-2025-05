#pragma once
#include <JuceHeader.h>

class MainComponent;

class NewProjectDialog : public juce::DialogWindow
{
public:
    NewProjectDialog(MainComponent& mainComp);
    void closeButtonPressed() override;

    class Content : public juce::Component,
                   public juce::Button::Listener
    {
    public:
        Content();
        ~Content() override;
        void resized() override;
        void paint(juce::Graphics& g) override;
        void buttonClicked(juce::Button* button) override;  // 버튼 클릭 핸들러

    private:
        // 프로젝트 이름 섹션
        juce::Label projectNameLabel;
        juce::TextEditor projectNameInput;

        // 연습 모드 선택 섹션
        juce::GroupComponent practiceTypeGroup;
        juce::ToggleButton songPracticeButton;
        juce::ToggleButton chromaticPracticeButton;
        juce::ToggleButton scalePracticeButton;

        // 추가 설정 섹션
        juce::GroupComponent advancedSettingsGroup;
        juce::Label bpmLabel;
        juce::Slider bpmSlider;
        juce::Label timeSignatureLabel;
        juce::ComboBox timeSignatureCombo;
        juce::Label audioInterfaceLabel;
        juce::ComboBox audioInterfaceCombo;

        // 하단 버튼
        juce::TextButton cancelButton;
        juce::TextButton createButton;

        void refreshAudioDeviceList();  // 오디오 장치 목록 갱신 함수 추가

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Content)
    };

    static void show(MainComponent& mainComp);

private:
    MainComponent& mainComponent;  // reference 사용
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectDialog)
}; 