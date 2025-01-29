#include "MainComponent.h"

MainComponent::MainComponent()
{
    // 고정 해상도 설정
    setSize(1440, 900);

    transitionProgress = 0.0f;
    showScreen(Screen::Start);
}

MainComponent::~MainComponent()
{
    stopTimer();
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xFF121212)); // Material dark background

    if (isTransitioning)
    {
        float easedProgress = getEasedProgress();

        // 현재 화면의 그림자
        if (currentScreen != nullptr)
        {
            auto screenBounds = currentScreen->getBounds().toFloat();

            if (transitionDirection == TransitionDirection::Right)
            {
                g.setGradientFill(juce::ColourGradient(
                    juce::Colours::black.withAlpha(0.3f * (1.0f - easedProgress)),
                    screenBounds.getRight(), 0.0f,
                    juce::Colours::transparentBlack,
                    screenBounds.getRight() + shadowSize, 0.0f,
                    false));
            }
            else
            {
                g.setGradientFill(juce::ColourGradient(
                    juce::Colours::black.withAlpha(0.3f * (1.0f - easedProgress)),
                    screenBounds.getX(), 0.0f,
                    juce::Colours::transparentBlack,
                    screenBounds.getX() - shadowSize, 0.0f,
                    false));
            }
            g.fillRect(getLocalBounds().toFloat());
        }

        // 다음 화면의 그림자
        if (nextScreen != nullptr)
        {
            auto screenBounds = nextScreen->getBounds().toFloat();

            if (transitionDirection == TransitionDirection::Right)
            {
                g.setGradientFill(juce::ColourGradient(
                    juce::Colours::black.withAlpha(0.3f * easedProgress),
                    screenBounds.getX(), 0.0f,
                    juce::Colours::transparentBlack,
                    screenBounds.getX() - shadowSize, 0.0f,
                    false));
            }
            else
            {
                g.setGradientFill(juce::ColourGradient(
                    juce::Colours::black.withAlpha(0.3f * easedProgress),
                    screenBounds.getRight(), 0.0f,
                    juce::Colours::transparentBlack,
                    screenBounds.getRight() + shadowSize, 0.0f,
                    false));
            }
            g.fillRect(getLocalBounds().toFloat());
        }
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    if (currentScreen != nullptr)
    {
        if (isTransitioning)
        {
            float easedProgress = getEasedProgress();

            int offset = static_cast<int>((transitionDirection == TransitionDirection::Right)
                                              ? -bounds.getWidth() * easedProgress
                                              : bounds.getWidth() * easedProgress);

            currentScreen->setBounds(bounds.translated(offset, 0));
        }
        else
        {
            currentScreen->setBounds(bounds);
        }
    }

    if (nextScreen != nullptr)
    {
        float easedProgress = getEasedProgress();

        int offset = static_cast<int>((transitionDirection == TransitionDirection::Right)
                                          ? bounds.getWidth() * (1.0f - easedProgress)
                                          : -bounds.getWidth() * (1.0f - easedProgress));

        nextScreen->setBounds(bounds.translated(offset, 0));
    }
}

void MainComponent::showScreen(Screen screen)
{
    std::unique_ptr<juce::Component> newScreen;

    switch (screen)
    {
    case Screen::Start:
        newScreen = std::make_unique<StartScreenComponent>([this](Screen nextScreen)
                                                           { showScreen(nextScreen); });
        break;

    case Screen::Record:
        newScreen = std::make_unique<RecordingComponent>();
        break;

    case Screen::Home:
        newScreen = std::make_unique<HomeComponent>([this](Screen nextScreen)
                                                    { showScreen(nextScreen); });
        break;

    case Screen::Upload:
        // TODO: Implement UploadComponent
        showScreen(Screen::Start);
        return;
    }

    if (newScreen != nullptr)
    {
        if (currentScreen == nullptr)
        {
            currentScreen = std::move(newScreen);
            addAndMakeVisible(currentScreen.get());
            resized();
        }
        else
        {
            // Record는 오른쪽으로, Start로 돌아갈 때는 왼쪽으로 슬라이드
            TransitionDirection direction = (screen == Screen::Start)
                                                ? TransitionDirection::Left
                                                : TransitionDirection::Right;
            startSlideTransition(std::move(newScreen), direction);
        }
        currentScreenType = screen;
    }
}

void MainComponent::startSlideTransition(std::unique_ptr<juce::Component> newScreen, TransitionDirection direction)
{
    nextScreen = std::move(newScreen);
    addAndMakeVisible(nextScreen.get());

    transitionProgress = 0.0f;
    isTransitioning = true;
    transitionDirection = direction;

    resized();
    startTimerHz(60);
}

void MainComponent::timerCallback()
{
    if (isTransitioning)
    {
        transitionProgress += transitionSpeed;

        if (transitionProgress >= 1.0f)
        {
            transitionProgress = 1.0f;
            isTransitioning = false;
            stopTimer();

            removeChildComponent(currentScreen.get());
            currentScreen = std::move(nextScreen);
            nextScreen = nullptr;

            resized();
        }
        else
        {
            resized();
        }
    }
}
