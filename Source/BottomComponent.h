#pragma once
#include <JuceHeader.h>

class BottomComponent : public juce::Component
{
public:
    BottomComponent();
    void resized() override;

private:
    class CourseListModel;  // 전방 선언
    
    juce::Label title;
    juce::ListBox courseList;
    juce::TextButton viewAllButton;
    std::unique_ptr<CourseListModel> courseListModel;
}; 