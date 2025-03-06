#pragma once
#include <JuceHeader.h>

class WaveformGraph : public juce::Component {
public:
    WaveformGraph();
    ~WaveformGraph() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setWaveforms(const juce::AudioBuffer<float>& original, 
                      const juce::AudioBuffer<float>& played);

    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    juce::AudioBuffer<float> originalWave;
    juce::AudioBuffer<float> playedWave;

    std::vector<juce::Point<float>> fullWaveformPoints; // 전체 캐시
    std::vector<juce::Point<float>> waveformPoints;     // 표시용
    float zoomLevel = 1.0f;
    float zoomPosition = 0.0f;
    float screenWidth = 0.0f;

    juce::TextButton playButton{"Play"};
    juce::TextButton stopButton{"Stop"};

    void computeFullWaveformPoints(); // 전체 계산
    void updateWaveformPoints();      // 표시 범위 조정

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformGraph)
};