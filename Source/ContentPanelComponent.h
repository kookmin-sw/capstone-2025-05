#pragma once
#include <JuceHeader.h>

class ContentPanelComponent : public juce::Component
{
public:
    ContentPanelComponent();
    void resized() override;

private:
    class SimpleListModel : public juce::ListBoxModel
    {
    public:
        int getNumRows() override { return 3; }
        
        void paintListBoxItem(int rowNumber, juce::Graphics& g, 
                            int width, int height, bool rowIsSelected) override
        {
            if (rowIsSelected)
                g.fillAll(juce::Colours::lightblue);

            g.setColour(juce::Colours::black);
            g.drawText(juce::String::fromUTF8("곡 ") + juce::String(char('A' + rowNumber)),
                      0, 0, width, height,
                      juce::Justification::centredLeft);
        }
    };

    juce::Label recentTitle;
    juce::ListBox recentList;
    juce::TextButton viewAllRecent;
    SimpleListModel recentListModel;

    juce::Label recommendTitle;
    juce::ListBox recommendList;
    juce::TextButton viewAllRecommend;
    SimpleListModel recommendListModel;
}; 