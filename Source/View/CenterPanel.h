#pragma once
#include <JuceHeader.h>
#include "Controller/AmpliTubeProcessor.h"
#include "IPanelComponent.h"

class CenterPanel : public juce::Component, public IPanelComponent {
public:
    CenterPanel();
    ~CenterPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // IPanelComponent 인터페이스 구현
    void initialize() override;
    void updatePanel() override;
    void resetPanel() override;
    juce::Component* asComponent() override { return this; }

private:
    juce::Image albumCover; // 앨범 커버 이미지 (나중에 로드)
    std::unique_ptr<AmpliTubeProcessor> ampliTubeProcessor; // unique_ptr로 변경
    std::unique_ptr<juce::AudioProcessorEditor> ampliTubeEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CenterPanel)
};