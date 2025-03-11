#include "AudioModel.h"

void AudioModel::setCurrentInputLevel(float level)
{
    if (currentInputLevel != level)
    {
        currentInputLevel = level;
        listeners.call([this](Listener& l) { l.inputLevelChanged(currentInputLevel); });
    }
}

void AudioModel::setPlaying(bool isPlaying)
{
    if (playing != isPlaying)
    {
        playing = isPlaying;
        listeners.call([this](Listener& l) { l.playStateChanged(playing); });
    }
}

void AudioModel::setVolume(float newVolume)
{
    if (volume != newVolume)
    {
        volume = newVolume;
        listeners.call([this](Listener& l) { l.volumeChanged(volume); });
    }
}