#include "CenterPanel.h"

CenterPanel::CenterPanel() {
    // 기본 앨범 커버 이미지 로드 (나중에 구현)
    // 예: albumCover = juce::ImageCache::getFromMemory(BinaryData::default_album_jpg, BinaryData::default_album_jpgSize);
}

CenterPanel::~CenterPanel() {}

void CenterPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey.brighter(0.1f));
    
    // 패널 제목 그리기
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Main View", getLocalBounds().reduced(10).removeFromTop(30),
              juce::Justification::centred, true);
    
    // 앨범 커버 이미지가 있으면 표시
    if (!albumCover.isNull())
    {
        auto bounds = getLocalBounds().reduced(50);
        g.drawImageWithin(albumCover, bounds.getX(), bounds.getY(), 
                         bounds.getWidth(), bounds.getHeight(),
                         juce::RectanglePlacement::centred);
    }
    else
    {
        // 앨범 커버가 없는 경우 빈 영역 표시
        auto bounds = getLocalBounds().reduced(50);
        g.setColour(juce::Colours::darkgrey);
        g.fillRect(bounds);
        g.setColour(juce::Colours::white);
        g.drawRect(bounds, 1);
        g.setFont(14.0f);
        g.drawText("No album cover", bounds, juce::Justification::centred, true);
    }
}

void CenterPanel::resized() {
    // 여기서는 별도의 컴포넌트 배치가 필요 없음
}