#include "CenterPanel.h"

CenterPanel::CenterPanel() {
    // AmpliTubeProcessor 생성
    ampliTubeProcessor = std::make_unique<AmpliTubeProcessor>();
    
    // AmpliTube 에디터 추가
    ampliTubeEditor.reset(ampliTubeProcessor->createEditor());
    if (ampliTubeEditor != nullptr)
    {
        addAndMakeVisible(ampliTubeEditor.get());
        DBG("AmpliTube editor added to MainComponent");
    }
    else
    {
        DBG("Failed to add AmpliTube editor");
    }

    DBG("MainComponent initialization finished!");
}

CenterPanel::~CenterPanel() {}

void CenterPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey); // 배경 회색 (구분용)
}

void CenterPanel::resized() {
    // AmpliTube 에디터 크기 조정
    if (ampliTubeEditor != nullptr)
    {
        ampliTubeEditor->setBounds(getLocalBounds().reduced(10));
    }
}