#include "CenterPanel.h"

CenterPanel::CenterPanel() {
    // 앨범 커버는 나중에 로드할 거라 빈 상태로
}

CenterPanel::~CenterPanel() {}

void CenterPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey); // 배경 회색 (구분용)
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Album or Score Here", getLocalBounds(), 
               juce::Justification::centred, true); // 임시 텍스트
}

void CenterPanel::resized() {
    // 지금은 빈 공간, 나중에 이미지나 악보 추가
}