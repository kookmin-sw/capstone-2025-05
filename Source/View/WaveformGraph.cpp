#include "WaveformGraph.h"
// Only include PracticeSongComponent if you actually need it
// #include "../PracticeSongComponent.h"

WaveformGraph::WaveformGraph()
{
    // Implementation
}

WaveformGraph::~WaveformGraph() {}

void WaveformGraph::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black);

    auto bounds = getLocalBounds().toFloat();
    float height = bounds.getHeight() - 40;
    float midY = height / 2;
    float amplitudeScale = height * 0.95f;

    if (!waveformPoints.empty())
    {
        g.setColour(juce::Colours::blue);
        juce::Path wavePath;
        wavePath.startNewSubPath(waveformPoints[0].getX(), midY - waveformPoints[0].getY() * amplitudeScale);
        for (size_t i = 1; i < waveformPoints.size(); ++i)
        {
            float x = waveformPoints[i].getX();
            if (x >= 0 && x <= screenWidth)
            {
                float y = midY - waveformPoints[i].getY() * amplitudeScale;
                wavePath.lineTo(x, y);
            }
        }
        g.strokePath(wavePath, juce::PathStrokeType(1.5f));
    }
    else
    {
        g.setColour(juce::Colours::white);
        g.drawText("No waveform loaded", bounds, juce::Justification::centred);
    }
}

void WaveformGraph::resized()
{
    auto bounds = getLocalBounds();
    auto buttonArea = bounds.removeFromBottom(40);
    playButton.setBounds(buttonArea.removeFromLeft(80));
    stopButton.setBounds(buttonArea.removeFromLeft(80));
    screenWidth = bounds.getWidth();
    updateWaveformPoints();
}

void WaveformGraph::setWaveforms(const juce::AudioBuffer<float> &original,
                                 const juce::AudioBuffer<float> &played)
{
    originalWave = original;
    playedWave = played;

    if (originalWave.getNumSamples() > 0)
    {
        DBG("WaveformGraph received - Channels: " << originalWave.getNumChannels()
                                                  << ", Samples: " << originalWave.getNumSamples());
        const float *data = originalWave.getReadPointer(0);
        DBG("First 5 samples: " << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3] << ", " << data[4]);
    }
    computeFullWaveformPoints();
    updateWaveformPoints();
    repaint();
}

void WaveformGraph::computeFullWaveformPoints()
{
    fullWaveformPoints.clear();
    if (originalWave.getNumSamples() == 0)
        return;

    int numSamples = originalWave.getNumSamples();
    int numPoints = juce::jmin(100000, numSamples); // 100만 개—고정밀 캐시

    const float *data = originalWave.getReadPointer(0);
    float maxAmplitude = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        maxAmplitude = juce::jmax(maxAmplitude, std::abs(data[i]));
    }
    float scaleFactor = (maxAmplitude > 0.0f) ? 1.0f / maxAmplitude : 1.0f;
    DBG("Max amplitude: " << maxAmplitude << ", Scale factor: " << scaleFactor);

    float step = numSamples / (float)numPoints;
    for (int i = 0; i < numPoints; ++i)
    {
        float sampleStart = i * step;
        float sampleEnd = juce::jmin(sampleStart + step, (float)numSamples);

        int startIdx = int(sampleStart);
        int endIdx = int(sampleEnd);
        float minVal = data[startIdx];
        float maxVal = data[startIdx];
        for (int j = startIdx + 1; j < endIdx; ++j)
        {
            minVal = juce::jmin(minVal, data[j]);
            maxVal = juce::jmax(maxVal, data[j]);
        }
        float avgVal = (minVal + maxVal) / 2.0f * scaleFactor;

        float x = i / (float)(numPoints - 1); // 0~1 정규화
        fullWaveformPoints.push_back({x, avgVal});
    }
    DBG("Computed full waveform points: " << fullWaveformPoints.size());
}

void WaveformGraph::updateWaveformPoints()
{
    waveformPoints.clear();
    if (fullWaveformPoints.empty())
        return;

    int totalPoints = fullWaveformPoints.size();
    float visibleFraction = 1.0f / zoomLevel;
    float startPos = zoomPosition * (totalPoints - 1);
    float endPos = startPos + visibleFraction * (totalPoints - 1);
    int startPoint = juce::jmax(0, int(startPos));
    int endPoint = juce::jmin(totalPoints - 1, int(endPos));

    int visiblePoints = endPoint - startPoint + 1;
    int numPoints = juce::jmin(int(screenWidth * 2), visiblePoints); // 화면 2배—부드럽게
    float step = visiblePoints / (float)numPoints;

    waveformPoints.reserve(numPoints); // 메모리 할당 최적화
    for (int i = 0; i < numPoints; ++i)
    {
        float idx = startPoint + (i * step);
        int lowerIdx = juce::jlimit(0, totalPoints - 1, int(idx));
        int upperIdx = juce::jlimit(0, totalPoints - 1, juce::jmin(lowerIdx + 1, totalPoints - 1));
        float fraction = idx - lowerIdx;

        float y = juce::jmap(fraction, 0.0f, 1.0f,
                             fullWaveformPoints[lowerIdx].getY(),
                             fullWaveformPoints[upperIdx].getY());
        float x = (i / (float)(numPoints - 1)) * screenWidth;
        waveformPoints.push_back({x, y});
    }
    DBG("Updated waveform points: " << waveformPoints.size());
}

void WaveformGraph::mouseWheelMove(const juce::MouseEvent &e, const juce::MouseWheelDetails &wheel)
{
    float oldZoom = zoomLevel;
    float delta = wheel.deltaY * 0.5f; // 감도 조정—부드럽게
    zoomLevel = juce::jlimit(1.0f, 100.0f, zoomLevel - delta);

    float mouseX = e.x / screenWidth;
    float visibleFraction = 1.0f / oldZoom;
    float mouseSamplePos = zoomPosition + (mouseX * visibleFraction);
    float newVisibleFraction = 1.0f / zoomLevel;
    zoomPosition = mouseSamplePos - (mouseX * newVisibleFraction);
    zoomPosition = juce::jlimit(0.0f, 1.0f - newVisibleFraction, zoomPosition);

    updateWaveformPoints();
    repaint();
}

void WaveformGraph::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        float mouseX = e.x / screenWidth;
        zoomPosition = mouseX - (0.5f / zoomLevel);
        zoomPosition = juce::jlimit(0.0f, 1.0f - (1.0f / zoomLevel), zoomPosition);
        updateWaveformPoints();
        repaint();
    }
}

void WaveformGraph::mouseDrag(const juce::MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        float deltaX = -e.getDistanceFromDragStartX() / (screenWidth * zoomLevel);
        zoomPosition = juce::jlimit(0.0f, 1.0f - (1.0f / zoomLevel), zoomPosition + deltaX);
        updateWaveformPoints();
        repaint();
    }
}