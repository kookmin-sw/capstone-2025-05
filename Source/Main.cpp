#include <JuceHeader.h>
#include "MainComponent.h"

// 윈도우(창) 클래스
class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow()
      : DocumentWindow ("MyProject",
                        juce::Colours::lightgrey,
                        DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new MainComponent(), true);
        centreWithSize(1920, 1200);
        setVisible(true);
    }

    void closeButtonPressed() override
    {
        // 창 닫으면 애플리케이션 종료
        juce::JUCEApplicationBase::quit();
    }
};

// JUCEApplication 상속
class MAPLEApplication : public juce::JUCEApplication
{
public:
    MAPLEApplication() {}

    const juce::String getApplicationName() override       { return "MAPLE"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise (const juce::String&) override
    {
        mainWindow.reset (new MainWindow());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void anotherInstanceStarted (const juce::String&) override {}

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (MAPLEApplication)
