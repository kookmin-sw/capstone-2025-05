#pragma once
#include <JuceHeader.h>

class AudioSettingsDialog : public juce::DialogWindow
{
public:
    AudioSettingsDialog(juce::AudioDeviceManager& deviceManager)
        : DialogWindow(juce::String::fromUTF8("오디오 설정"),
                      juce::Colours::transparentBlack, true, true)
    {
        setContentOwned(new Content(deviceManager), true);
        centreWithSize(500, 600);
        setResizable(true, false);
    }

    void closeButtonPressed() override
    {
        exitModalState(0);
        delete this;
    }

    class Content : public juce::Component,
                   public juce::ChangeListener
    {
    public:
        Content(juce::AudioDeviceManager& deviceManager)
            : audioDeviceManager(deviceManager)
        {
            // 1. 기본 UI 컴포넌트 초기화
            addAndMakeVisible(diagnosticsBox);
            diagnosticsBox.setMultiLine(true);
            diagnosticsBox.setReturnKeyStartsNewLine(true);
            diagnosticsBox.setReadOnly(true);
            diagnosticsBox.setScrollbarsShown(true);
            diagnosticsBox.setCaretVisible(false);
            diagnosticsBox.setPopupMenuEnabled(true);

            // 2. 다이얼로그가 표시된 후 오디오 설정 UI 초기화
            juce::Timer::callAfterDelay(100, [this]() {
                audioSetupComp.reset(new juce::AudioDeviceSelectorComponent(audioDeviceManager,
                                                                          0, 256,
                                                                          0, 256,
                                                                          true,
                                                                          true,
                                                                          true,
                                                                          false));
                addAndMakeVisible(audioSetupComp.get());
                audioDeviceManager.addChangeListener(this);
                logMessage("Audio device diagnostics:\n");
                dumpDeviceInfo();
                resized();
            });
        }

        ~Content() override
        {
            audioDeviceManager.removeChangeListener(this);
        }

        void resized() override
        {
            auto r = getLocalBounds().reduced(4);
            if (audioSetupComp != nullptr)  // nullptr 체크 추가
            {
                audioSetupComp->setBounds(r.removeFromTop(proportionOfHeight(0.65f)));
            }
            diagnosticsBox.setBounds(r);
        }

        void changeListenerCallback(juce::ChangeBroadcaster*) override
        {
            dumpDeviceInfo();
        }

    private:
        juce::AudioDeviceManager& audioDeviceManager;
        std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSetupComp;
        juce::TextEditor diagnosticsBox;

        void dumpDeviceInfo()
        {
            logMessage("--------------------------------------");
            logMessage("Current audio device type: " + (audioDeviceManager.getCurrentDeviceTypeObject() != nullptr
                                                    ? audioDeviceManager.getCurrentDeviceTypeObject()->getTypeName()
                                                    : "<none>"));

            if (auto* device = audioDeviceManager.getCurrentAudioDevice())
            {
                logMessage("Current audio device: "   + device->getName().quoted());
                logMessage("Sample rate: "    + juce::String(device->getCurrentSampleRate()) + " Hz");
                logMessage("Block size: "     + juce::String(device->getCurrentBufferSizeSamples()) + " samples");
                logMessage("Output Latency: " + juce::String(device->getOutputLatencyInSamples()) + " samples");
                logMessage("Input Latency: "  + juce::String(device->getInputLatencyInSamples()) + " samples");
                logMessage("Bit depth: "      + juce::String(device->getCurrentBitDepth()));
                logMessage("Input channel names: "    + device->getInputChannelNames().joinIntoString(", "));
                logMessage("Active input channels: "  + getListOfActiveBits(device->getActiveInputChannels()));
                logMessage("Output channel names: "   + device->getOutputChannelNames().joinIntoString(", "));
                logMessage("Active output channels: " + getListOfActiveBits(device->getActiveOutputChannels()));
            }
            else
            {
                logMessage("No audio device open");
            }
        }

        void logMessage(const juce::String& m)
        {
            diagnosticsBox.moveCaretToEnd();
            diagnosticsBox.insertTextAtCaret(m + juce::newLine);
        }

        static juce::String getListOfActiveBits(const juce::BigInteger& b)
        {
            juce::StringArray bits;

            for (int i = 0; i <= b.getHighestBit(); ++i)
                if (b[i])
                    bits.add(juce::String(i));

            return bits.joinIntoString(", ");
        }
    };

    static void show(juce::AudioDeviceManager& deviceManager)
    {
        AudioSettingsDialog* dialog = new AudioSettingsDialog(deviceManager);
        dialog->enterModalState(true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSettingsDialog)
}; 