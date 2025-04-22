#include "TopBar.h"
#include "GuitarPracticeComponent.h" // Now include the full header here

TopBar::TopBar(GuitarPracticeComponent &parent)
    : parentComponent(parent)
{
    addAndMakeVisible(recordButton);
    addAndMakeVisible(loadButton);
    
    // 버튼 이벤트 처리
    loadButton.onClick = [this]() {
        // 파일 선택 다이얼로그 구현
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file",
            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
            "*.wav;*.mp3;*.aif"
        );
        
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode, 
            [this](const juce::FileChooser& chooser) {
                juce::File selectedFile = chooser.getResult();
                if (selectedFile.existsAsFile()) {
                    // 선택된 파일 처리
                    // parentComponent.loadAudioFile(selectedFile);
                    DBG("Selected file: " + selectedFile.getFullPathName());
                }
            }
        );
    };
    
    // 기타 초기화
    initialize();
}

TopBar::~TopBar() {}

void TopBar::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::darkgrey);
    
    // 제목 표시
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Guitar Practice Mode", getLocalBounds(), juce::Justification::centred, true);
}

void TopBar::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // 왼쪽에 버튼 배치
    recordButton.setBounds(bounds.removeFromLeft(80));
    bounds.removeFromLeft(10); // 간격
    loadButton.setBounds(bounds.removeFromLeft(100));
}

// IPanelComponent 인터페이스 구현
void TopBar::initialize()
{
    // 패널 초기화
    recordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    recordButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    
    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
}

void TopBar::updatePanel()
{
    // 패널 상태 업데이트
    repaint();
}

void TopBar::resetPanel()
{
    // 패널 상태 초기화
    initialize();
}