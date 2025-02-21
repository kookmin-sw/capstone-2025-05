#pragma once
#include <JuceHeader.h>

class PracticeSongComponent : public juce::AudioAppComponent
{
public:
    PracticeSongComponent();
    ~PracticeSongComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    class HeaderPanel : public juce::Component
    {
    public:
        HeaderPanel()
        {
            projectTitle.setText(juce::String::fromUTF8("프로젝트: 재즈 연습 1"), juce::dontSendNotification);
            songTitle.setText(juce::String::fromUTF8("곡: Autumn Leaves"), juce::dontSendNotification);
            projectTitle.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
            songTitle.setFont(juce::Font(juce::FontOptions(18.0f)));
            
            backButton.setButtonText(juce::String::fromUTF8("이전"));
            menuButton.setButtonText(juce::String::fromUTF8("메뉴"));
            settingsButton.setButtonText(juce::String::fromUTF8("설정"));
            
            addAndMakeVisible(projectTitle);
            addAndMakeVisible(songTitle);
            addAndMakeVisible(backButton);
            addAndMakeVisible(menuButton);
            addAndMakeVisible(settingsButton);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            const int buttonWidth = 40;
            
            // 좌측 버튼들
            backButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(5));
            menuButton.setBounds(bounds.removeFromLeft(buttonWidth).reduced(5));
            
            // 우측 버튼
            settingsButton.setBounds(bounds.removeFromRight(buttonWidth).reduced(5));
            
            // 타이틀
            auto titleArea = bounds.reduced(10, 0);
            projectTitle.setBounds(titleArea.removeFromTop(30));
            songTitle.setBounds(titleArea);
        }

    private:
        juce::Label projectTitle, songTitle;
        juce::TextButton backButton, menuButton, settingsButton;
    };

    class ControlPanel : public juce::Component
    {
    public:
        ControlPanel(bool isLeft)
        {
            if (isLeft)
            {
                // 좌측 패널 (오디오 컨트롤)
                volumeLabel.setText(juce::String::fromUTF8("Volume"), juce::dontSendNotification);
                volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
                volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
                
                recordButton.setButtonText(juce::String::fromUTF8("녹음"));
                recordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
                playButton.setButtonText(juce::String::fromUTF8("재생"));
                stopButton.setButtonText(juce::String::fromUTF8("정지"));
                
                addAndMakeVisible(volumeLabel);
                addAndMakeVisible(volumeSlider);
                addAndMakeVisible(recordButton);
                addAndMakeVisible(playButton);
                addAndMakeVisible(stopButton);
                addAndMakeVisible(levelMeter);
            }
            else
            {
                // 우측 패널 (연습 설정)
                tempoLabel.setText(juce::String::fromUTF8("Tempo"), juce::dontSendNotification);
                tempoSlider.setSliderStyle(juce::Slider::LinearHorizontal);
                tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
                tempoSlider.setRange(40, 240, 1);
                tempoSlider.setValue(120);
                
                metronomeButton.setButtonText(juce::String::fromUTF8("Metronome"));
                metronomeButton.setToggleState(true, juce::dontSendNotification);
                
                addAndMakeVisible(tempoLabel);
                addAndMakeVisible(tempoSlider);
                addAndMakeVisible(metronomeButton);
            }
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced(10);
            
            if (volumeSlider.isVisible()) // 좌측 패널
            {
                volumeLabel.setBounds(bounds.removeFromTop(20));
                volumeSlider.setBounds(bounds.removeFromLeft(50).withHeight(200));
                
                auto buttonArea = bounds.removeFromTop(150);
                const int buttonHeight = 40;
                recordButton.setBounds(buttonArea.removeFromTop(buttonHeight).reduced(5));
                playButton.setBounds(buttonArea.removeFromTop(buttonHeight).reduced(5));
                stopButton.setBounds(buttonArea.removeFromTop(buttonHeight).reduced(5));
                
                levelMeter.setBounds(bounds.reduced(5));
            }
            else // 우측 패널
            {
                tempoLabel.setBounds(bounds.removeFromTop(20));
                tempoSlider.setBounds(bounds.removeFromTop(40));
                bounds.removeFromTop(10);
                metronomeButton.setBounds(bounds.removeFromTop(30));
            }
        }

    private:
        // 좌측 패널 컴포넌트
        juce::Label volumeLabel;
        juce::Slider volumeSlider;
        juce::TextButton recordButton, playButton, stopButton;
        juce::Component levelMeter; // TODO: 실제 레벨미터 구현
        
        // 우측 패널 컴포넌트
        juce::Label tempoLabel;
        juce::Slider tempoSlider;
        juce::ToggleButton metronomeButton;
    };

    HeaderPanel headerPanel;
    ControlPanel leftPanel{true}, rightPanel{false};
    juce::Component centralContent;  // TODO: 악보/앨범커버 표시 영역
    juce::Component waveformDisplay;  // TODO: 파형 표시 영역

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PracticeSongComponent)
}; 