#pragma once
#include <JuceHeader.h>
#include "../Model/AudioModel.h"

class LeftPanel : public juce::Component, public AudioModel::Listener {
public:
    LeftPanel(AudioModel& model);
    ~LeftPanel() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // AudioModel::Listener implementation
    void inputLevelChanged(float newLevel) override;
    
private:
    AudioModel& audioModel;
    
    double progressValue;  // Add this member variable
    std::unique_ptr<juce::Label> levelLabel;
    std::unique_ptr<juce::ProgressBar> levelMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeftPanel)
};