#pragma once
#include <JuceHeader.h>

class BottomComponent : public juce::Component
{
public:
    BottomComponent();
    void resized() override;

private:
    class CourseListModel : public juce::ListBoxModel
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

    juce::Label title;
    juce::ListBox courseList;
    juce::TextButton viewAllButton;
    CourseListModel courseListModel;
}; 