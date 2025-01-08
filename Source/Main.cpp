#include <JuceHeader.h>
#include "MainComponent.h"

/**
    ���ø����̼� ��Ʈ�� ����Ʈ
    - JUCEApplication�� ��ӹ޾� initialise(), shutdown() ����
    - ���� �����츦 ������ MainComponent�� ǥ��
*/
class MainApplication : public juce::JUCEApplication
{
public:
    MainApplication() {}

    const juce::String getApplicationName() override { return "AudioAppComponent Example"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow("AudioAppComponent Example", new MainComponent(), *this));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String&) override {}

    //==============================================================================
    // ���� ������ ����
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name, juce::Component* c, JUCEApplication& appRef)
            : DocumentWindow(
                name,
                juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons),
            app(appRef)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(c, true);

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(true, false);
            centreWithSize(getWidth(), getHeight());
#endif
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

    private:
        JUCEApplication& app;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

// ���ø����̼� ���� ��ũ��
START_JUCE_APPLICATION(MainApplication)
