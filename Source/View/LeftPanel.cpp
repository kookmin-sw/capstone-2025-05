#include "LeftPanel.h"

LeftPanel::LeftPanel(AudioModel& model)
    : audioModel(model), 
      progressValue(0.0)
{
    // 입력 레벨 관련 UI 설정
    levelLabel = std::make_unique<juce::Label>("levelLabel", "Input Level:");
    levelLabel->setJustificationType(juce::Justification::left);
    addAndMakeVisible(levelLabel.get());
    
    // progressValue 포인터를 사용하여 ProgressBar 생성
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
    // 리스너를 먼저 제거해 새로운 이벤트 구독을 방지
    audioModel.removeListener(this);
    
    // 미처리된 메시지들이 안전하게 처리될 수 있도록 컴포넌트 참조 해제
    volumeSlider = nullptr;
    levelMeter = nullptr;
    levelLabel = nullptr;
    volumeLabel = nullptr;
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
        if (volumeSlider != nullptr)
            volumeSlider->setValue(vol, juce::dontSendNotification);
    });
}

void LeftPanel::onPositionChanged(double positionInSeconds)
{
    // 위치가 변경되면 UI를 업데이트 (필요하다면)
    // 현재는 특별한 처리가 필요하지 않음
}

void LeftPanel::onInputLevelChanged(float newLevel)
{
    // 입력 레벨이 변경되면 레벨미터 업데이트
    progressValue = newLevel;
    
    // JUCE 컴포넌트 스레드에서 안전하게 UI 업데이트
    if (juce::MessageManager::getInstance()->isThisTheMessageThread())
    {
        // 이미 메시지 스레드라면 직접 업데이트
        DBG("LeftPanel: Input Level changed to " + juce::String(newLevel));
        if (levelMeter != nullptr)
            levelMeter->repaint();
    }
    else
    {
        // 다른 스레드에서 호출되었다면 메시지 스레드로 전달
        juce::MessageManager::callAsync([this, level = newLevel]() {
            // 객체가 아직 유효한지는 직접 확인할 수 없으므로 
            // lambda가 호출될 때 객체가 유효하다고 가정
            // 소멸자에서 컴포넌트 포인터들을 nullptr로 설정하여 안전성 확보
            DBG("LeftPanel: Input Level changed to " + juce::String(level));
            if (levelMeter != nullptr)
                levelMeter->repaint();
        });
    }
}