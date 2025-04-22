#include "LeftPanel.h"

LeftPanel::LeftPanel(AudioModel& model)
    : audioModel(model), 
      progressValue(0.0)
{
    // 입력 레벨 관련 UI 설정
    levelLabel = std::make_unique<juce::Label>("levelLabel", "Input Level:");
    levelLabel->setJustificationType(juce::Justification::left);
    addAndMakeVisible(levelLabel.get());
    
    levelMeter = std::make_unique<juce::ProgressBar>(progressValue);
    addAndMakeVisible(levelMeter.get());
    levelMeter->setTextToDisplay("");
    
    // 볼륨 컨트롤 UI 설정
    volumeLabel = std::make_unique<juce::Label>("volumeLabel", "Volume:");
    volumeLabel->setJustificationType(juce::Justification::left);
    addAndMakeVisible(volumeLabel.get());
    
    volumeSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight);
    volumeSlider->setRange(0.0, 1.0, 0.01);
    volumeSlider->setValue(audioModel.getVolume());
    volumeSlider->onValueChange = [this]() {
        audioModel.setVolume((float)volumeSlider->getValue());
    };
    addAndMakeVisible(volumeSlider.get());
    
    // 새로운 인터페이스를 사용하여 모델에 리스너 등록
    audioModel.addListener(this);
}

LeftPanel::~LeftPanel()
{
    // 모델에서 리스너 제거
    audioModel.removeListener(this);
}

void LeftPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    
    // 패널 제목 그리기
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Controls", getLocalBounds().reduced(10).removeFromTop(30),
               juce::Justification::centred, true);
}

void LeftPanel::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // 패널 제목을 위한 공간
    bounds.removeFromTop(40);
    
    // 입력 레벨 컨트롤
    levelLabel->setBounds(bounds.removeFromTop(20));
    levelMeter->setBounds(bounds.removeFromTop(30));
    
    bounds.removeFromTop(20); // 간격 추가
    
    // 볼륨 컨트롤
    volumeLabel->setBounds(bounds.removeFromTop(20));
    volumeSlider->setBounds(bounds.removeFromTop(30));
}

// IAudioModelListener 인터페이스 구현
void LeftPanel::onPlayStateChanged(bool isPlaying)
{
    // 재생 상태가 변경되면 UI를 업데이트 (필요하다면)
    // 현재는 특별한 처리가 필요하지 않음
}

void LeftPanel::onVolumeChanged(float newVolume)
{
    // 볼륨이 변경되면 슬라이더 값 업데이트
    juce::MessageManager::callAsync([this, vol = newVolume]() {
        volumeSlider->setValue(vol, juce::dontSendNotification);
    });
}

void LeftPanel::onPositionChanged(double positionInSeconds)
{
    // 위치가 변경되면 UI를 업데이트 (필요하다면)
    // 현재는 특별한 처리가 필요하지 않음
}