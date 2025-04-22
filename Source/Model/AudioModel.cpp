#include "AudioModel.h"

void AudioModel::setCurrentInputLevel(float level)
{
    if (currentInputLevel != level)
    {
        currentInputLevel = level;
        notifyInputLevelChanged();
    }
}

void AudioModel::setPlaying(bool isPlaying)
{
    if (playing != isPlaying)
    {
        playing = isPlaying;
        notifyPlayStateChanged();
    }
}

void AudioModel::setVolume(float newVolume)
{
    if (volume != newVolume)
    {
        volume = newVolume;
        notifyVolumeChanged();
    }
}

void AudioModel::setCurrentPosition(double positionInSeconds)
{
    if (currentPositionInSeconds != positionInSeconds)
    {
        currentPositionInSeconds = positionInSeconds;
        notifyPositionChanged();
    }
}