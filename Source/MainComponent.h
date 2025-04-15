#pragma once
#include <JuceHeader.h>
#include "gp_parser.h"
#include "ContentPanelComponent.h"

class HeaderComponent;
class MainActionComponent;
class BottomComponent;
class PracticeSongComponent;

class MainComponent : public juce::Component,
                      public ContentPanelComponent::SongSelectedListener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void showMainScreen();
    void showPracticeScreen();
    
    // ContentPanelComponent::SongSelectedListener 구현
    void songSelected(const juce::String& songId) override;

    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }
    
    // 현재 선택된 곡 ID 반환
    juce::String getSelectedSongId() const { return selectedSongId; }

private:
    juce::AudioDeviceManager deviceManager;
    std::unique_ptr<HeaderComponent> headerComponent;
    std::unique_ptr<MainActionComponent> mainActionComponent;
    std::unique_ptr<ContentPanelComponent> contentPanelComponent;
    std::unique_ptr<BottomComponent> bottomComponent;
    std::unique_ptr<PracticeSongComponent> practiceSongComponent;
    
    juce::String selectedSongId; // 현재 선택된 곡 ID

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};