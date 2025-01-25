#include "StartScreenComponent.h"

StartScreenComponent::StartScreenComponent(std::function<void(Screen)> callback)
    : screenChangeCallback(std::move(callback))
{
    // Set up fonts
    titleFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 72.0f, juce::Font::plain);
    descriptionFont = juce::Font(juce::Font::getDefaultSansSerifFontName(), 20.0f, juce::Font::plain);

    // Enable mouse events
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addMouseListener(this, true);

    // Load logo image
    auto logoFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getChildFile("Resources/Images/start-screen.png");
                        
    if (logoFile.existsAsFile())
    {
        logoImage = juce::ImageFileFormat::loadFrom(logoFile);
    }
}

StartScreenComponent::~StartScreenComponent()
{
}

void StartScreenComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);

    auto bounds = getLocalBounds();
    auto leftHalf = bounds.removeFromLeft(bounds.getWidth() / 2);

    // Left panel: logo image
    if (logoImage.isValid())
    {
        g.drawImage(logoImage, 
                   leftHalf.reduced(50).toFloat(),
                   juce::RectanglePlacement::centred);
    }

    // Right panel
    drawRightPanel(g, bounds);

    // Draw login button
    drawLoginButton(g);
}

void StartScreenComponent::drawRightPanel(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto area = bounds.reduced(50);
    
    // Calculate total content height
    const int totalContentHeight = 100 + 20 + 1 + 40 + 100;  // title + top margin + line + middle margin + description
    
    // Calculate vertical center alignment starting position
    int startY = (area.getHeight() - totalContentHeight) / 2;
    area.setY(area.getY() + startY);
    
    // Project title
    g.setFont(titleFont);
    g.setColour(juce::Colour(0xFF333333));
    auto titleBounds = area.removeFromTop(100);
    g.drawText("MAPLE", titleBounds, juce::Justification::centred, true);

    // Divider line
    area.removeFromTop(20);
    g.setColour(juce::Colour(0x33000000));
    auto lineArea = area.removeFromTop(1);
    g.fillRect(lineArea.withSizeKeepingCentre(area.getWidth() * 0.8f, 1));

    // Project description
    area.removeFromTop(40);
    g.setFont(descriptionFont);
    g.setColour(juce::Colour(0xFF666666));
    auto descriptionBounds = area.removeFromTop(100);
    
    g.drawFittedText(projectDescription, 
                    descriptionBounds,
                    juce::Justification::centred,
                    3);
}

void StartScreenComponent::drawLoginButton(juce::Graphics& g)
{
    // Improved shadow effect
    float elevation = isLoginButtonMouseOver ? 12.0f : 6.0f;
    auto shadowBounds = loginButtonBounds.toFloat().translated(0.0f, elevation);
    
    // Smooth shadow effect
    for (float i = 0.0f; i < elevation; i += 0.5f)
    {
        float alpha = (elevation - i) / (elevation * 2.0f);
        g.setColour(juce::Colours::black.withAlpha(alpha * 0.2f));
        g.drawRoundedRectangle(shadowBounds.expanded(i + 2), 10.0f, 1.0f);
    }
    
    // Button background
    auto buttonBounds = loginButtonBounds.toFloat();
    if (!isLoginButtonMouseOver)
        buttonBounds = buttonBounds.translated(0.0f, elevation);
        
    // White background with subtle gradient effect
    juce::ColourGradient gradient(
        juce::Colours::white,
        buttonBounds.getTopLeft(),
        juce::Colours::white.withBrightness(0.95f),
        buttonBounds.getBottomLeft(),
        false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(buttonBounds, 10.0f);
    
    // White border
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(buttonBounds, 10.0f, 1.0f);
    
    // Button text with hover effect
    juce::Colour textColour;
    if (isLoginButtonDown)
        textColour = juce::Colour(0xFF666666);  // Darker grey when pressed
    else if (isLoginButtonMouseOver)
        textColour = juce::Colour(0xFF808080);  // Lighter grey on hover
    else
        textColour = juce::Colour(0xFF999999);  // Default grey

    g.setColour(textColour);
    g.setFont(descriptionFont.withHeight(16.0f));
    g.drawText("LOGIN", buttonBounds, juce::Justification::centred, false);
}

void StartScreenComponent::resized()
{
    auto bounds = getLocalBounds();
    auto rightHalf = bounds.removeFromRight(bounds.getWidth() / 2);
    
    // Position login button below content
    auto buttonArea = rightHalf.reduced(50);
    auto contentHeight = 100 + 20 + 1 + 40 + 100;
    int startY = (buttonArea.getHeight() - contentHeight) / 2;
    buttonArea.removeFromTop(startY + contentHeight + 50);
    
    // Set wider button size
    loginButtonBounds = buttonArea.removeFromTop(50).withSizeKeepingCentre(300, 50);  // width increased to 300
}

void StartScreenComponent::mouseMove(const juce::MouseEvent& event)
{
    bool wasOver = isLoginButtonMouseOver;
    isLoginButtonMouseOver = isMouseOverButton(event.position);
    
    if (wasOver != isLoginButtonMouseOver)
    {
        repaint();
    }
}

void StartScreenComponent::mouseEnter(const juce::MouseEvent& event)
{
    bool wasOver = isLoginButtonMouseOver;
    isLoginButtonMouseOver = isMouseOverButton(event.position);
    
    if (wasOver != isLoginButtonMouseOver)
    {
        repaint();
    }
}

void StartScreenComponent::mouseExit(const juce::MouseEvent& event)
{
    isLoginButtonMouseOver = false;
    repaint();
}

void StartScreenComponent::mouseDown(const juce::MouseEvent& event)
{
    if (isMouseOverButton(event.position))
    {
        isLoginButtonDown = true;
        repaint();
    }
}

void StartScreenComponent::mouseUp(const juce::MouseEvent& event)
{
    if (isLoginButtonDown && isMouseOverButton(event.position))
    {
        // Login ë²„íŠ¼ ?´ë¦­ ??Home ?”ë©´?¼ë¡œ ?´ë™
        screenChangeCallback(Screen::Home);
    }
    isLoginButtonDown = false;
    repaint();
}

bool StartScreenComponent::isMouseOverButton(const juce::Point<float>& position) const
{
    return loginButtonBounds.toFloat().contains(position);
} 