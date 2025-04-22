#include "HeaderComponent.h"
#include "Dialog/AudioSettingsDialog.h"
#include "MainComponent.h"

HeaderComponent::HeaderComponent()
{
    title.setText(juce::String::fromUTF8("MAPLE: Music Analysis & Performance Learning Environment"), 
                 juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(24)));
    addAndMakeVisible(title);

    // 메뉴 버튼들 생성
    menuButtons.push_back(std::make_unique<juce::TextButton>(juce::String::fromUTF8("File")));
    menuButtons.push_back(std::make_unique<juce::TextButton>(juce::String::fromUTF8("Edit")));
    menuButtons.push_back(std::make_unique<juce::TextButton>(juce::String::fromUTF8("View")));
    menuButtons.push_back(std::make_unique<juce::TextButton>(juce::String::fromUTF8("Help")));

    for (auto& button : menuButtons)
    {
        addAndMakeVisible(*button);
    }

    // 우측 버튼들
    profileButton.setButtonText(juce::String::fromUTF8("Profile"));
    settingsButton.setButtonText(juce::String::fromUTF8("⚙Settings"));

    settingsButton.onClick = [this]()
    {
        if (auto* mainComp = findParentComponentOfClass<MainComponent>())
        {
            AudioSettingsDialog::show(mainComp->getDeviceManager());
        }
    };

    addAndMakeVisible(profileButton);
    addAndMakeVisible(settingsButton);
}

void HeaderComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // 타이틀 영역
    title.setBounds(bounds.removeFromTop(40));
    
    // 메뉴 바 영역
    auto menuBar = bounds.removeFromTop(30);
    int buttonWidth = 80;
    
    // 좌측 메뉴 버튼들
    for (auto& button : menuButtons)
    {
        button->setBounds(menuBar.removeFromLeft(buttonWidth));
    }
    
    // 우측 버튼들
    settingsButton.setBounds(menuBar.removeFromRight(buttonWidth));
    profileButton.setBounds(menuBar.removeFromRight(buttonWidth));
} 