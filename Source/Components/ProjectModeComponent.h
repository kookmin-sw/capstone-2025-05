#pragma once
#include <JuceHeader.h>
#include "../UI/Styles/MapleColours.h"
#include "../UI/Styles/MapleTypography.h"

class ProjectModeComponent : public juce::Component
{
public:
    ProjectModeComponent()
    {
        setOpaque(true);
        
        // 앨범 커버
        albumCover = std::make_unique<AlbumCover>();
        addAndMakeVisible(*albumCover);
        
        // 볼륨 컨트롤
        volumeSlider = std::make_unique<juce::Slider>();
        volumeSlider->setSliderStyle(juce::Slider::LinearVertical);
        volumeSlider->setRange(0.0, 1.0);
        volumeSlider->setValue(0.7);
        volumeSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        volumeSlider->setColour(juce::Slider::trackColourId, juce::Colours::grey);
        volumeSlider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
        addAndMakeVisible(*volumeSlider);

        // 속도 컨트롤
        speedSlider = std::make_unique<juce::Slider>();
        speedSlider->setSliderStyle(juce::Slider::LinearVertical);
        speedSlider->setRange(0.5, 2.0);
        speedSlider->setValue(1.0);
        speedSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        speedSlider->setColour(juce::Slider::trackColourId, juce::Colours::grey);
        speedSlider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
        addAndMakeVisible(*speedSlider);

        // 트랜스포트 컨트롤
        playButton = std::make_unique<juce::TextButton>(juce::CharPointer_UTF8(u8"재생"));
        pauseButton = std::make_unique<juce::TextButton>(juce::CharPointer_UTF8(u8"일시정지"));
        recordButton = std::make_unique<juce::TextButton>(juce::CharPointer_UTF8(u8"녹음"));
        
        addAndMakeVisible(*playButton);
        addAndMakeVisible(*pauseButton);
        addAndMakeVisible(*recordButton);

        // 파형 뷰어
        waveformViewer = std::make_unique<WaveformViewer>();
        addAndMakeVisible(*waveformViewer);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleColours::currentTheme.background);
        
        // 곡 정보 표시
        auto bounds = getLocalBounds();
        auto infoArea = bounds.removeFromTop(bounds.getHeight() / 2)
                             .removeFromBottom(60);
        
        g.setColour(juce::Colours::white);
        g.setFont(MapleTypography::getPretendardMedium(14.0f));
        g.drawText(u8"곡 제목: Sample Track", infoArea.removeFromTop(20), 
                  juce::Justification::centred);
        g.drawText(u8"아티스트: Unknown", infoArea.removeFromTop(20),
                  juce::Justification::centred);
        g.drawText(u8"BPM: 120 | 길이: 3:45", infoArea,
                  juce::Justification::centred);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // 앨범 커버 (중앙 상단)
        const int coverSize = 200;
        albumCover->setBounds(bounds.getCentreX() - coverSize/2,
                            80,
                            coverSize,
                            coverSize);

        // 컨트롤 영역
        auto controlArea = bounds.reduced(20).removeFromTop(bounds.getHeight() / 2);
        
        // 볼륨 슬라이더 (왼쪽)
        const int sliderWidth = 40;
        const int sliderHeight = 200;
        volumeSlider->setBounds(50,
                              controlArea.getCentreY() - sliderHeight/2,
                              sliderWidth,
                              sliderHeight);

        // 속도 슬라이더 (오른쪽)
        speedSlider->setBounds(bounds.getRight() - 90,
                             controlArea.getCentreY() - sliderHeight/2,
                             sliderWidth,
                             sliderHeight);

        // 트랜스포트 컨트롤 (중앙)
        auto transportArea = controlArea.removeFromBottom(100);
        const int buttonWidth = 100;
        const int buttonHeight = 30;
        const int buttonSpacing = 20;
        
        auto buttonBounds = juce::Rectangle<int>(
            bounds.getCentreX() - (buttonWidth * 3 + buttonSpacing * 2) / 2,
            transportArea.getCentreY() - buttonHeight/2,
            buttonWidth,
            buttonHeight);
            
        playButton->setBounds(buttonBounds);
        buttonBounds.translate(buttonWidth + buttonSpacing, 0);
        pauseButton->setBounds(buttonBounds);
        buttonBounds.translate(buttonWidth + buttonSpacing, 0);
        recordButton->setBounds(buttonBounds);

        // 파형 뷰어 (하단)
        auto waveformArea = bounds.removeFromBottom(200).reduced(20);
        waveformViewer->setBounds(waveformArea);
    }

private:
    // 앨범 커버 컴포넌트
    class AlbumCover : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat().reduced(2);
            g.setColour(juce::Colour(40, 40, 40));
            g.fillRoundedRectangle(bounds, 10.0f);
            g.setColour(juce::Colours::grey);
            g.drawRoundedRectangle(bounds, 10.0f, 1.0f);
            
            // 임시 텍스트
            g.setColour(juce::Colours::white);
            g.setFont(16.0f);
            g.drawText(u8"앨범 커버", getLocalBounds(), 
                      juce::Justification::centred);
        }
    };

    // 파형 뷰어 컴포넌트
    class WaveformViewer : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();
            g.setColour(juce::Colour(30, 30, 30));
            g.fillRoundedRectangle(bounds, 5.0f);
            
            // 임시 텍스트
            g.setColour(juce::Colours::white);
            g.setFont(16.0f);
            g.drawText(juce::CharPointer_UTF8(u8"파형 뷰어"), getLocalBounds(),
                      juce::Justification::centred);
        }
    };

    std::unique_ptr<AlbumCover> albumCover;
    std::unique_ptr<juce::Slider> volumeSlider;
    std::unique_ptr<juce::Slider> speedSlider;
    std::unique_ptr<juce::TextButton> playButton;
    std::unique_ptr<juce::TextButton> pauseButton;
    std::unique_ptr<juce::TextButton> recordButton;
    std::unique_ptr<WaveformViewer> waveformViewer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectModeComponent)
}; 