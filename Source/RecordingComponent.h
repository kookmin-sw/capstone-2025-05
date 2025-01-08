#pragma once

#include <JuceHeader.h>
#include "SongSelectionComponent.h"

class RecordingComponent : public juce::Component
{
public:
    RecordingComponent();
    ~RecordingComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void onSongSelected([[maybe_unused]] const SongInfo& song);
    std::unique_ptr<SongSelectionComponent> songSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordingComponent)
}; 