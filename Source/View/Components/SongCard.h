#pragma once
#include <JuceHeader.h>
#include "../LookAndFeel/MapleTheme.h"

/**
 * Spotify 스타일의 노래/앨범 카드 컴포넌트
 * 커버 이미지와 타이틀을 표시합니다.
 */
class SongCard : public juce::Component,
                 public juce::Button::Listener
{
public:
    struct SongInfo {
        juce::String id;
        juce::String title;
        juce::String artist;
        juce::String albumTitle;
        juce::String duration;
        juce::File coverImageFile;
    };
    
    SongCard(const SongInfo& info)
    : songInfo(info)
    {
        // 커버 이미지 로드
        if (songInfo.coverImageFile.existsAsFile())
        {
            coverImage = juce::ImageFileFormat::loadFrom(songInfo.coverImageFile);
        }
        
        // 플레이 버튼 초기화
        playButton.setButtonText(">");
        playButton.setColour(juce::TextButton::buttonColourId, MapleTheme::getAccentColour());
        playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        playButton.addListener(this);
        playButton.setVisible(false); // 호버 상태에서만 표시
        addAndMakeVisible(playButton);
        
        titleLabel.setText(songInfo.title, juce::dontSendNotification);
        titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
        titleLabel.setColour(juce::Label::textColourId, MapleTheme::getTextColour());
        addAndMakeVisible(titleLabel);
        
        artistLabel.setText(songInfo.artist, juce::dontSendNotification);
        artistLabel.setFont(juce::Font(14.0f));
        artistLabel.setColour(juce::Label::textColourId, MapleTheme::getSubTextColour());
        addAndMakeVisible(artistLabel);
    }
    
    ~SongCard() override
    {
        playButton.removeListener(this);
    }
    
    void paint(juce::Graphics& g) override
    {
        // 카드 배경
        g.setColour(MapleTheme::getCardColour());
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
        
        // 호버 상태일 때 효과
        if (isMouseOver)
        {
            g.setColour(juce::Colours::white.withAlpha(0.05f));
            g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
        }
        
        // 커버 이미지 영역
        auto imageArea = getLocalBounds().withTrimmedBottom(getHeight() / 3);
        
        // 커버 이미지 그리기
        if (coverImage.isValid())
        {
            g.drawImage(coverImage, imageArea.toFloat(), juce::RectanglePlacement::centred);
        }
        else
        {
            // 이미지가 없는 경우 기본 플레이스홀더
            g.setColour(MapleTheme::getBackgroundColour().brighter(0.1f));
            g.fillRect(imageArea.toFloat());
            g.setColour(MapleTheme::getSubTextColour());
            g.drawText("No Image", imageArea, juce::Justification::centred);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto imageHeight = bounds.getHeight() * 2 / 3;
        bounds.removeFromTop(imageHeight);
        
        // 텍스트 영역
        auto textArea = bounds.reduced(8);
        titleLabel.setBounds(textArea.removeFromTop(20));
        artistLabel.setBounds(textArea.removeFromTop(18));
        
        // 플레이 버튼 (중앙에 배치)
        auto buttonSize = juce::jmin(60, getWidth() / 3);
        playButton.setBounds(getWidth() / 2 - buttonSize / 2, 
                             imageHeight - buttonSize / 2,
                             buttonSize, buttonSize);
        playButton.setShape(juce::Path(), true, true, false);
    }
    
    void mouseEnter(const juce::MouseEvent& event) override
    {
        isMouseOver = true;
        playButton.setVisible(true);
        repaint();
    }
    
    void mouseExit(const juce::MouseEvent& event) override
    {
        isMouseOver = false;
        playButton.setVisible(false);
        repaint();
    }
    
    void buttonClicked(juce::Button* button) override
    {
        if (button == &playButton)
        {
            // 카드가 선택되었을 때 이벤트 발생
            if (onCardSelected)
                onCardSelected(songInfo.id);
        }
    }
    
    void mouseUp(const juce::MouseEvent& event) override
    {
        // 카드가 클릭되었을 때도 이벤트 발생
        if (event.mouseWasClicked() && onCardSelected)
            onCardSelected(songInfo.id);
    }
    
    // 카드 선택 콜백 함수 설정
    void setOnCardSelectedCallback(std::function<void(juce::String)> callback)
    {
        onCardSelected = std::move(callback);
    }
    
private:
    SongInfo songInfo;
    juce::Image coverImage;
    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::ShapeButton playButton{"playButton", MapleTheme::getAccentColour(), 
                                MapleTheme::getAccentColour().brighter(0.2f),
                                MapleTheme::getAccentColour().darker(0.2f)};
    bool isMouseOver = false;
    
    std::function<void(juce::String)> onCardSelected;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SongCard)
}; 