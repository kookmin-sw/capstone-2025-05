#include "NewProjectDialog.h"
#include "MainComponent.h"

NewProjectDialog::NewProjectDialog(MainComponent& mainComp)
    : DialogWindow(juce::String::fromUTF8("새 프로젝트 생성"), 
                  juce::Colours::white, true, true),
      mainComponent(mainComp)
{
    setContentOwned(new Content(), true);
    centreWithSize(400, 500);
    setResizable(false, false);
}

NewProjectDialog::Content::Content()
{
    // 프로젝트 이름 설정
    projectNameLabel.setText(juce::String::fromUTF8("프로젝트 이름"), juce::dontSendNotification);
    projectNameInput.setTextToShowWhenEmpty("Untitled Project", juce::Colours::grey);
    addAndMakeVisible(projectNameLabel);
    addAndMakeVisible(projectNameInput);

    // 연습 모드 설정
    practiceTypeGroup.setText(juce::String::fromUTF8("연습 모드"));
    songPracticeButton.setButtonText(juce::String::fromUTF8("곡 연습"));
    chromaticPracticeButton.setButtonText(juce::String::fromUTF8("크로메틱 연습"));
    scalePracticeButton.setButtonText(juce::String::fromUTF8("스케일 연습"));
    
    songPracticeButton.setRadioGroupId(1);
    chromaticPracticeButton.setRadioGroupId(1);
    scalePracticeButton.setRadioGroupId(1);
    songPracticeButton.setToggleState(true, juce::dontSendNotification);

    addAndMakeVisible(practiceTypeGroup);
    addAndMakeVisible(songPracticeButton);
    addAndMakeVisible(chromaticPracticeButton);
    addAndMakeVisible(scalePracticeButton);

    // 추가 설정
    advancedSettingsGroup.setText(juce::String::fromUTF8("추가 설정"));
    
    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmSlider.setRange(40, 240, 1);
    bpmSlider.setValue(120);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    
    timeSignatureLabel.setText(juce::String::fromUTF8("타임시그니처"), juce::dontSendNotification);
    timeSignatureCombo.addItem("4/4", 1);
    timeSignatureCombo.addItem("3/4", 2);
    timeSignatureCombo.addItem("6/8", 3);
    timeSignatureCombo.setSelectedId(1);
    
    audioInterfaceLabel.setText(juce::String::fromUTF8("음성 입력 장치"), juce::dontSendNotification);
    refreshAudioDeviceList();  // 오디오 장치 목록 초기화

    addAndMakeVisible(advancedSettingsGroup);
    addAndMakeVisible(bpmLabel);
    addAndMakeVisible(bpmSlider);
    addAndMakeVisible(timeSignatureLabel);
    addAndMakeVisible(timeSignatureCombo);
    addAndMakeVisible(audioInterfaceLabel);
    addAndMakeVisible(audioInterfaceCombo);

    // 하단 버튼
    cancelButton.setButtonText(juce::String::fromUTF8("취소"));
    createButton.setButtonText(juce::String::fromUTF8("생성 / 시작"));
    createButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    
    addAndMakeVisible(cancelButton);
    addAndMakeVisible(createButton);

    // 버튼 리스너 추가
    cancelButton.addListener(this);
    createButton.addListener(this);
}

NewProjectDialog::Content::~Content()
{
    cancelButton.removeListener(this);
    createButton.removeListener(this);
}

void NewProjectDialog::Content::buttonClicked(juce::Button* button)
{
    if (button == &cancelButton || button == &createButton)
    {
        if (auto* dialog = findParentComponentOfClass<NewProjectDialog>())
        {
            if (button == &createButton)
            {
                // 프로젝트 생성 및 화면 전환
                dialog->mainComponent.showPracticeScreen();
            }
            
            dialog->exitModalState(button == &createButton ? 1 : 0);
            delete dialog;
        }
    }
}

void NewProjectDialog::Content::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    const int labelHeight = 25;
    const int spacing = 10;

    // 프로젝트 이름
    auto row = bounds.removeFromTop(labelHeight);
    projectNameLabel.setBounds(row.removeFromLeft(100));
    projectNameInput.setBounds(row);
    bounds.removeFromTop(spacing);

    // 연습 모드
    practiceTypeGroup.setBounds(bounds.removeFromTop(140));
    auto practiceTypeBounds = practiceTypeGroup.getBounds().reduced(10, 25);
    songPracticeButton.setBounds(practiceTypeBounds.removeFromTop(30));
    chromaticPracticeButton.setBounds(practiceTypeBounds.removeFromTop(30));
    scalePracticeButton.setBounds(practiceTypeBounds.removeFromTop(30));
    bounds.removeFromTop(spacing);

    // 추가 설정
    advancedSettingsGroup.setBounds(bounds.removeFromTop(150));
    auto settingsBounds = advancedSettingsGroup.getBounds().reduced(10, 25);
    
    auto bpmRow = settingsBounds.removeFromTop(labelHeight);
    bpmLabel.setBounds(bpmRow.removeFromLeft(50));
    bpmSlider.setBounds(bpmRow);
    
    settingsBounds.removeFromTop(spacing);
    auto timeRow = settingsBounds.removeFromTop(labelHeight);
    timeSignatureLabel.setBounds(timeRow.removeFromLeft(100));
    timeSignatureCombo.setBounds(timeRow);
    
    settingsBounds.removeFromTop(spacing);
    auto audioRow = settingsBounds.removeFromTop(labelHeight);
    audioInterfaceLabel.setBounds(audioRow.removeFromLeft(100));
    audioInterfaceCombo.setBounds(audioRow);

    // 하단 버튼
    bounds.removeFromTop(spacing * 2);
    auto buttonRow = bounds.removeFromTop(labelHeight);
    const int buttonWidth = 100;
    cancelButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    createButton.setBounds(buttonRow.removeFromRight(buttonWidth));
}

void NewProjectDialog::Content::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void NewProjectDialog::Content::refreshAudioDeviceList()
{
    audioInterfaceCombo.clear();
    
    int itemId = 1;
    
    if (audioInterfaceCombo.getNumItems() > 0)
        audioInterfaceCombo.setSelectedId(1);
}

void NewProjectDialog::show(MainComponent& mainComp)
{
    NewProjectDialog* dialog = new NewProjectDialog(mainComp);
    dialog->enterModalState(true);
}

void NewProjectDialog::closeButtonPressed()
{
    exitModalState(0);
    delete this;  // 다이얼로그 삭제
}
