#include "BottomComponent.h"

class BottomComponent::CourseListModel : public juce::ListBoxModel
{
public:
    int getNumRows() override { return 2; }
    
    void paintListBoxItem(int rowNumber, juce::Graphics& g, 
                         int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(juce::Colours::lightblue);

        g.setColour(juce::Colours::black);
        juce::String courseName = rowNumber == 0 ? 
            juce::String::fromUTF8("크로메틱 기초 (진행률 70%)") : 
            juce::String::fromUTF8("스케일 기초 (진행률 30%)");
        g.drawText(courseName, 0, 0, width, height, juce::Justification::centredLeft);
    }
};

BottomComponent::BottomComponent()
{
    title.setText(juce::String::fromUTF8("연습 코스 (COURSES)"), 
                 juce::dontSendNotification);
    courseListModel = std::make_unique<CourseListModel>();
    courseList.setModel(courseListModel.get());
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