#include <JuceHeader.h>
#include "View/MainComponent.h"
#include "Util/EnvLoader.h"

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
        // Make sure to close all components before quitting
        setVisible(false);
        
        // Call the prepareToClose method on MainComponent
        if (auto* mainComp = dynamic_cast<MainComponent*>(getContentComponent()))
            mainComp->prepareToClose();
            
        clearContentComponent();
        
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
        EnvLoader::load();
        mainWindow.reset (new MainWindow());
    }

    void shutdown() override
    {
        // Ensure all components are released before destroying the main window
        if (mainWindow != nullptr)
        {
            mainWindow->setVisible(false);
            mainWindow->clearContentComponent();
        }
        
        // Now safely release the main window
        mainWindow = nullptr;
    }

    void anotherInstanceStarted (const juce::String&) override {}

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (MAPLEApplication)
