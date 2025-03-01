#pragma once
#include <JuceHeader.h>

class MainComponent;  // 전방 선언

class PracticeSongComponent : public juce::Component, public juce::AudioIODeviceCallback
{
public:
    PracticeSongComponent(MainComponent& mainComp);
    ~PracticeSongComponent() override;

    // AudioIODeviceCallback 인터페이스 구현
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                              float* const* outputChannelData, int numOutputChannels, int numSamples,
                              const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // 레벨 미터 클래스
    class LevelMeterComponent : public juce::Component, private juce::Timer
    {
    public:
        LevelMeterComponent()
        {
            startTimerHz(24); // 초당 24번 업데이트
        }

        ~LevelMeterComponent() override
        {
            stopTimer();
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();
            
            // 배경
            g.setColour(juce::Colours::black);
            g.fillRect(bounds);
            
            // 레벨 표시
            const float levelHeight = bounds.getHeight() * level;
            juce::Rectangle<float> levelRect(bounds.getX(), bounds.getBottom() - levelHeight, 
                                           bounds.getWidth(), levelHeight);
            
            // 레벨에 따라 색상 변경
            if (level > 0.8f)
                g.setColour(juce::Colours::red);
            else if (level > 0.5f)
                g.setColour(juce::Colours::orange);
            else
                g.setColour(juce::Colours::green);
                
            g.fillRect(levelRect);
            
            // 눈금 표시
            g.setColour(juce::Colours::white);
            for (int i = 1; i < 10; ++i)
            {
                float y = bounds.getY() + bounds.getHeight() * i / 10.0f;
                g.drawLine(bounds.getX(), y, bounds.getRight(), y, 0.5f);
            }
        }

        void setLevel(float newLevel)
        {
            // 0.0 ~ 1.0 범위로 제한
            level = juce::jlimit(0.0f, 1.0f, newLevel);
        }

    private:
        float level = 0.0f;
        
        void timerCallback() override
        {
            // 레벨이 서서히 감소하도록 함
            level = level * 0.9f;
            repaint();
        }
    };

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
                
                // 레벨 미터 - 남은 공간의 왼쪽 부분 사용
                levelMeter.setBounds(bounds.removeFromLeft(30).reduced(5));
            }
            else // 우측 패널
            {
                tempoLabel.setBounds(bounds.removeFromTop(20));
                tempoSlider.setBounds(bounds.removeFromTop(40));
                bounds.removeFromTop(10);
                metronomeButton.setBounds(bounds.removeFromTop(30));
            }
        }

        LevelMeterComponent& getLevelMeter() { return levelMeter; }

    private:
        // 좌측 패널 컴포넌트
        juce::Label volumeLabel;
        juce::Slider volumeSlider;
        juce::TextButton recordButton, playButton, stopButton;
        LevelMeterComponent levelMeter; // 레벨 미터 구현
        
        // 우측 패널 컴포넌트
        juce::Label tempoLabel;
        juce::Slider tempoSlider;
        juce::ToggleButton metronomeButton;
    };

    HeaderPanel headerPanel;
    ControlPanel leftPanel{true}, rightPanel{false};
    juce::Component centralContent;  // TODO: 악보/앨범커버 표시 영역
    juce::Component waveformDisplay;  // TODO: 파형 표시 영역
    
    MainComponent& mainComponent;  // MainComponent에 대한 참조
    
    // 오디오 레벨 계산을 위한 변수
    float currentInputLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PracticeSongComponent)
}; 