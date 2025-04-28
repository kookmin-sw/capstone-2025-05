#include "RecordingThumbnail.h"

RecordingThumbnail::RecordingThumbnail()
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
}

RecordingThumbnail::~RecordingThumbnail()
{
    thumbnail.removeChangeListener(this);
}

void RecordingThumbnail::setSource(const juce::File& audioFile)
{
    thumbnail.setSource(new juce::FileInputSource(audioFile));
}

juce::AudioThumbnail& RecordingThumbnail::getAudioThumbnail()
{
    return thumbnail;
}

void RecordingThumbnail::setDisplayFullThumbnail(bool displayFull)
{
    displayFullThumb = displayFull;
    repaint();
}

void RecordingThumbnail::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);
    
    if (thumbnail.getTotalLength() > 0.0)
    {
        auto endTime = displayFullThumb ? thumbnail.getTotalLength()
                                       : juce::jmax(30.0, thumbnail.getTotalLength());
        
        auto thumbArea = getLocalBounds().reduced(2);
        thumbnail.drawChannels(g, thumbArea, 0.0, endTime, 1.0f);
    }
    else
    {
        g.setFont(14.0f);
        g.drawFittedText("(No Recording)", getLocalBounds(), juce::Justification::centred, 2);
    }
}

void RecordingThumbnail::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail)
        repaint();
} 