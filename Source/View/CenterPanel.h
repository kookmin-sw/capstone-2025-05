#pragma once
#include <JuceHeader.h>
#include "../AmpliTubeProcessor.h"

class CenterPanel : public juce::Component {
public:
    CenterPanel();
    ~CenterPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Image albumCover; // 앨범 커버 이미지 (나중에 로드)
    AmpliTubeProcessor ampliTubeProcessor;
    std::unique_ptr<juce::AudioProcessorEditor> ampliTubeEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CenterPanel)
};