#include "LeftPanel.h"

LeftPanel::LeftPanel(AudioModel& model)
    : audioModel(model), 
      progressValue(0.0),  // Add this member variable
      levelMeter(std::make_unique<juce::ProgressBar>(progressValue))  // Use the member variable
{
    // Set up components
    levelLabel = std::make_unique<juce::Label>("levelLabel", "Input Level:");
    addAndMakeVisible(levelLabel.get());
    
    addAndMakeVisible(levelMeter.get());
    levelMeter->setTextToDisplay("");
    
    // Register as listener for model changes
    audioModel.addListener(this);
}

LeftPanel::~LeftPanel()
{
    // Unregister to avoid callbacks to deleted objects
    audioModel.removeListener(this);
}

void LeftPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void LeftPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    levelLabel->setBounds(bounds.removeFromTop(20));
    levelMeter->setBounds(bounds.removeFromTop(30));
}

void LeftPanel::inputLevelChanged(float newLevel)
{
    // Update UI on the message thread to avoid threading issues
    juce::MessageManager::callAsync([this, level = newLevel]() {
        // Update the progressValue that the ProgressBar is monitoring
        progressValue = level;
        levelMeter->repaint();
    });
}