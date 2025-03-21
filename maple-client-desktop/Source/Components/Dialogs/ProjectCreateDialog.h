#pragma once
#include <JuceHeader.h>
#include "../../UI/Styles/MapleColours.h"

class ProjectCreateDialog : public juce::Component
{
public:
    ProjectCreateDialog()
    {
        // 프로젝트 이름 입력
        addAndMakeVisible(nameLabel);
        addAndMakeVisible(nameInput);
        nameLabel.setText("프로젝트 이름:", juce::dontSendNotification);
        
        // 연습 모드 선택 (라디오 버튼)
        addAndMakeVisible(modeGroup);
        
        auto addRadioButton = [this](const juce::String& text) {
            auto button = std::make_unique<juce::ToggleButton>(text);
            addAndMakeVisible(*button);
            radioButtons.push_back(std::move(button));
        };

        addRadioButton("곡 연습");
        addRadioButton("스케일 연습");
        addRadioButton("크로메틱 연습");
        
        // 확인/취소 버튼
        addAndMakeVisible(createButton);
        addAndMakeVisible(cancelButton);
        createButton.setButtonText("프로젝트 생성");
        cancelButton.setButtonText("취소");
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(20);
        auto row = bounds.removeFromTop(30);
        
        nameLabel.setBounds(row.removeFromLeft(100));
        nameInput.setBounds(row);
        
        bounds.removeFromTop(20);
        modeGroup.setBounds(bounds.removeFromTop(120));
        
        bounds.removeFromTop(20);
        auto buttonRow = bounds.removeFromTop(30);
        createButton.setBounds(buttonRow.removeFromLeft(buttonRow.getWidth() / 2 - 5));
        cancelButton.setBounds(buttonRow.removeFromRight(buttonRow.getWidth() - 5));
    }

private:
    juce::Label nameLabel;
    juce::TextEditor nameInput;
    juce::GroupComponent modeGroup{"", "연습 모드"};
    std::vector<std::unique_ptr<juce::ToggleButton>> radioButtons;
    juce::TextButton createButton, cancelButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectCreateDialog)
}; 