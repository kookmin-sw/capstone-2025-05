#include "RecordingComponent.h"

RecordingComponent::RecordingComponent()
{
    songSelector = std::make_unique<SongSelectionComponent>([this](const SongInfo& song) {
        onSongSelected(song);
    });
    addAndMakeVisible(songSelector.get());
}

RecordingComponent::~RecordingComponent()
{
}

void RecordingComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF121212));
}

void RecordingComponent::resized()
{
    songSelector->setBounds(getLocalBounds());
}

void RecordingComponent::onSongSelected([[maybe_unused]] const SongInfo& song)
{
    // TODO: Implement recording screen for the selected song
    // This will be implemented in the next step
} 