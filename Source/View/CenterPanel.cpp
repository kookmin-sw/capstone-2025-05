#include "CenterPanel.h"

CenterPanel::CenterPanel() {
    // AmpliTube 에디터 추가
    ampliTubeEditor.reset(ampliTubeProcessor.createEditor());
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
    // g.setColour(juce::Colours::white);
    // g.setFont(20.0f);
    // g.drawText("Album or Score Here", getLocalBounds(), 
    //            juce::Justification::centred, true); // 임시 텍스트
}

void CenterPanel::resized() {
    // 지금은 빈 공간, 나중에 이미지나 악보 추가
}