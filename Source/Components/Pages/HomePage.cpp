#include "HomePage.h"
#include "../../Utils/Constants.h"

HomePage::HomePage()
{
    // Title 설정
    addAndMakeVisible(titleLabel);
    titleLabel.setText("MAPLE", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(32.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, Constants::Colors::text);

    // Welcome 메시지
    addAndMakeVisible(welcomeLabel);
    welcomeLabel.setText("Welcome back!", juce::dontSendNotification);
    welcomeLabel.setFont(juce::Font(24.0f));
    welcomeLabel.setJustificationType(juce::Justification::centred);
    welcomeLabel.setColour(juce::Label::textColourId, Constants::Colors::text);

    // Continue 버튼
    addAndMakeVisible(continueButton);
    continueButton.setText("Continue Learning");
    continueButton.setColour(juce::TextButton::buttonColourId, Constants::Colors::primary);
    continueButton.setColour(juce::TextButton::textColourOffId, Constants::Colors::text);

    // New Course 버튼
    addAndMakeVisible(newCourseButton);
    newCourseButton.setText("Start New Course");
    newCourseButton.setColour(juce::TextButton::buttonColourId, Constants::Colors::primary);
    newCourseButton.setColour(juce::TextButton::textColourOffId, Constants::Colors::text);
}

HomePage::~HomePage()
{
}

void HomePage::paint(juce::Graphics &g)
{
    g.fillAll(Constants::Colors::background);
}

void HomePage::resized()
{
    auto bounds = getLocalBounds();
    const int padding = 20;
    const int buttonHeight = 40;
    const int buttonWidth = 200;

    // Title 위치 설정
    titleLabel.setBounds(bounds.removeFromTop(100));

    // Welcome 메시지 위치 설정
    welcomeLabel.setBounds(bounds.removeFromTop(50));

    // 버튼들을 중앙에 배치
    auto buttonBounds = bounds.withSizeKeepingCentre(buttonWidth, buttonHeight * 2 + padding);
    continueButton.setBounds(buttonBounds.removeFromTop(buttonHeight));
    buttonBounds.removeFromTop(padding); // 버튼 사이 간격
    newCourseButton.setBounds(buttonBounds);
}