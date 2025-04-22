#include "RecordingThumbnail.h"

RecordingThumbnail::RecordingThumbnail()
{
    formatManager.registerBasicFormats();
    thumbnailCache = std::make_unique<juce::AudioThumbnailCache>(10);
    thumbnail = std::make_unique<juce::AudioThumbnail>(512, formatManager, *thumbnailCache);
    thumbnail->addChangeListener(this);
}

RecordingThumbnail::~RecordingThumbnail()
{
    thumbnail->removeChangeListener(this);
}

void RecordingThumbnail::setSource(const juce::File& audioFile)
{
    thumbnail->setSource(new juce::FileInputSource(audioFile));
}

juce::AudioThumbnail& RecordingThumbnail::getAudioThumbnail()
{
    return *thumbnail;
}

void RecordingThumbnail::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::lightblue);
    
    if (thumbnail->getTotalLength() > 0.0)
    {
        auto bounds = getLocalBounds();
        auto thumbArea = bounds.reduced(2);
        thumbnail->drawChannels(g, thumbArea, 0.0, thumbnail->getTotalLength(), 1.0f);
    }
    else
    {
        g.setFont(14.0f);
        g.drawFittedText("No recording loaded", getLocalBounds(), juce::Justification::centred, 2);
    }
}

void RecordingThumbnail::changeListenerCallback(juce::ChangeBroadcaster*)
{
    // 썸네일이 변경되면 리페인트
    repaint();
} 