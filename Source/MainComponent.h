#pragma once
#include <JuceHeader.h>

class HeaderComponent;
class MainActionComponent;
class ContentPanelComponent;
class BottomComponent;
class PracticeSongComponent;

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void showMainScreen();
    void showPracticeScreen();

private:
    juce::AudioDeviceManager deviceManager;
    std::unique_ptr<HeaderComponent> headerComponent;
    std::unique_ptr<MainActionComponent> mainActionComponent;
    std::unique_ptr<ContentPanelComponent> contentPanelComponent;
    std::unique_ptr<BottomComponent> bottomComponent;
    std::unique_ptr<PracticeSongComponent> practiceSongComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};