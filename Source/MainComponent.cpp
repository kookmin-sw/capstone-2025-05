#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(800, 400);
    
    // Settings ��ư �ʱ�ȭ
    settingsButton.setButtonText("Settings");
    settingsButton.addListener(this);
    addAndMakeVisible(settingsButton);
    
    // ����� ���� �ʱ�ȭ
    setAudioChannels(1, 2);  // ��� �Է�, ���׷��� ���
    
    // ���� �ʱ�ȭ
    audioBuffer.fill(0.0f);
    
    // ȭ�� ���� Ÿ�̸� ���� (60fps)
    startTimerHz(60);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &settingsButton)
    {
        auto audioSettingsComp = std::make_unique<juce::AudioDeviceSelectorComponent>(
            deviceManager,
            0, 2,  // �Է� ä��
            0, 2,  // ��� ä��
            true,  // ����� �Է� ǥ��
            true,  // ����� ��� ǥ��
            false, // ä�� ���� ����
            false  // MIDI ����
        );

        audioSettingsComp->setSize(500, 400);

        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(audioSettingsComp.release());
        options.dialogTitle = "Audio Settings";
        options.dialogBackgroundColour = juce::Colours::darkgrey;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = false;

        settingsWindow = std::shared_ptr<juce::DialogWindow>(options.launchAsync());
    }
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // ����� ���� �� �ʿ��� �ʱ�ȭ
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // �Է� ä�ο��� ������ �б�
    auto* inputChannel = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
    
    // ���ۿ� ������ ����
    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
        audioBuffer[bufferIndex] = inputChannel[i];
        bufferIndex = (bufferIndex + 1) % bufferSize;
    }
    
    // ����� ���� ó��
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // ����� ���� �� ����
}

void MainComponent::paint(juce::Graphics& g)
{
    // ��� �׸���
    g.fillAll(juce::Colours::black);
    
    auto width = (float)getWidth();
    auto height = (float)getHeight();
    auto centerY = height * 0.5f;
    
    // ���� �׸���
    g.setColour(juce::Colours::lime);
    
    juce::Path waveform;
    waveform.startNewSubPath(0, centerY);
    
    // ������ �����͸� ����Ͽ� ���� �׸���
    for (int x = 0; x < width; ++x)
    {
        // x ��ǥ�� �ش��ϴ� ���� �ε��� ���
        int index = (bufferIndex + (int)((float)x * bufferSize / width)) % bufferSize;
        
        // y ��ǥ ��� (������ ȭ�� ������ 40%�� �����ϸ�)
        float y = centerY + (audioBuffer[index] * height * 0.4f);
        
        if (x == 0)
            waveform.startNewSubPath(0, y);
        else
            waveform.lineTo(x, y);
    }
    
    // �ε巯�� ������ ���� �׸���
    g.strokePath(waveform, juce::PathStrokeType(2.0f));
}

void MainComponent::resized()
{
    // Settings ��ư ��ġ ����
    settingsButton.setBounds(10, 10, 100, 30);
}

void MainComponent::timerCallback()
{
    // ȭ�� ����
    repaint();
}
