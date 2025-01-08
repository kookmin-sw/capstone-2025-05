#include <JuceHeader.h>
#include "MainComponent.h"

/**
    애플리케이션 엔트리 포인트
    - JUCEApplication을 상속받아 initialise(), shutdown() 구현
    - 메인 윈도우를 생성해 MainComponent를 표시
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
    // 메인 윈도우 정의
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

// 애플리케이션 실행 매크로
START_JUCE_APPLICATION(MainApplication)
