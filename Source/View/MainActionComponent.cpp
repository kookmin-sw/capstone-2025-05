#include "MainActionComponent.h"
#include "Dialog/NewProjectDialog.h"
#include "MainComponent.h"

MainActionComponent::MainActionComponent(MainComponent& mainComp)
    : mainComponent(mainComp)
{
    startButton.setButtonText(juce::String::fromUTF8("프로젝트(연주) 시작 +"));
    startButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    addAndMakeVisible(startButton);
    startButton.onClick = [this]() {
        NewProjectDialog::show(mainComponent);
    };

    subtitle.setText(juce::String::fromUTF8("새 프로젝트 생성 / 새 연습 세션 시작"), 
                    juce::dontSendNotification);
    subtitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(subtitle);
}

void MainActionComponent::resized()
{
    auto bounds = getLocalBounds();
    auto buttonHeight = 60;
    
    startButton.setBounds(bounds.withSizeKeepingCentre(200, buttonHeight));
    subtitle.setBounds(bounds.removeFromBottom(30));
} 