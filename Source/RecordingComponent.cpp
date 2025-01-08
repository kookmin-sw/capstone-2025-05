#include "RecordingComponent.h"

RecordingComponent::RecordingComponent()
{
    songSelector = std::make_unique<SongSelectionComponent>([this](const SongInfo& song) {
        onSongSelected(song);
    });
    addAndMakeVisible(songSelector.get());
    setAlpha(1.0f);
}

RecordingComponent::~RecordingComponent()
{
}

void RecordingComponent::paint(juce::Graphics& g)
{
}

void RecordingComponent::resized()
{
    if (songSelector != nullptr)
    {
        songSelector->setBounds(getLocalBounds());
    }
}

void RecordingComponent::onSongSelected([[maybe_unused]] const SongInfo& song)
{
    // TODO: Implement recording screen for the selected song
} 