#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(800, 600);
    showScreen(Screen::Start);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    if (currentScreen != nullptr)
        currentScreen->setBounds(getLocalBounds());
}

void MainComponent::showScreen(Screen screen)
{
    if (currentScreen != nullptr)
    {
        removeChildComponent(currentScreen.get());
        currentScreen = nullptr;
    }

    switch (screen)
    {
        case Screen::Start:
            currentScreen = std::make_unique<StartScreenComponent>([this](Screen nextScreen)
            {
                showScreen(nextScreen);
            });
            break;
            
        case Screen::Record:
            // TODO: RecordingComponent 구현 필요
            showScreen(Screen::Start);
            break;
            
        case Screen::Upload:
            // TODO: UploadComponent 구현 필요
            showScreen(Screen::Start);
            break;
    }

    if (currentScreen != nullptr)
    {
        addAndMakeVisible(currentScreen.get());
        currentScreenType = screen;
        resized();
    }
}
