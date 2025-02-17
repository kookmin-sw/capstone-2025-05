#include "BottomComponent.h"

BottomComponent::BottomComponent()
{
    title.setText(juce::String::fromUTF8("연습 코스 (COURSES)"), 
                 juce::dontSendNotification);
    courseList.setModel(&courseListModel);
    viewAllButton.setButtonText(juce::String::fromUTF8("모두 보기"));
    
    addAndMakeVisible(title);
    addAndMakeVisible(courseList);
    addAndMakeVisible(viewAllButton);
}

void BottomComponent::resized()
{
    auto bounds = getLocalBounds();
    title.setBounds(bounds.removeFromTop(30));
    viewAllButton.setBounds(bounds.removeFromBottom(30));
    courseList.setBounds(bounds);
} 