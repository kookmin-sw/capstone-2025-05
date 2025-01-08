#include "StartScreenComponent.h"

StartScreenComponent::StartScreenComponent(std::function<void(Screen)> callback)
    : screenChangeCallback(std::move(callback))
{
    // Setup buttons
    recordButton.setButtonText("Record");
    recordButton.onClick = [this]() { screenChangeCallback(Screen::Record); };
    addAndMakeVisible(recordButton);
    
    uploadButton.setButtonText("Upload");
    uploadButton.onClick = [this]() { screenChangeCallback(Screen::Upload); };
    addAndMakeVisible(uploadButton);

    // Add mouse listeners for hover effects
    recordButton.addMouseListener(this, false);
    uploadButton.addMouseListener(this, false);
}

StartScreenComponent::~StartScreenComponent()
{
}

void StartScreenComponent::paint(juce::Graphics& g)
{
    // Background gradient
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(0xFF1E1E1E),
        0.0f, 0.0f,
        juce::Colour(0xFF2D2D2D),
        0.0f, (float)getHeight(),
        false));
    g.fillAll();

    // Draw MAPLE logo
    g.setFont(logoFont);
    g.setColour(juce::Colours::white);
    auto logoBounds = getLocalBounds().removeFromTop(120);
    g.drawText("MAPLE", logoBounds, juce::Justification::centred, true);

    // Draw custom buttons
    auto buttonArea = getLocalBounds().withTrimmedTop(120);
    auto leftHalf = buttonArea.removeFromLeft(buttonArea.getWidth() / 2);
    
    drawButton(g, recordButton.getBounds(), "Record", recordButtonMouseOver);
    drawButton(g, uploadButton.getBounds(), "Upload", uploadButtonMouseOver);
}

void StartScreenComponent::drawButton(juce::Graphics& g, juce::Rectangle<int> bounds, 
                                    const juce::String& text, bool isMouseOver)
{
    // Button background with gradient
    juce::ColourGradient gradient(
        juce::Colour(0xFF4A90E2),
        bounds.getX(), bounds.getY(),
        juce::Colour(0xFF357ABD),
        bounds.getX(), bounds.getBottom(),
        false);
    
    if (isMouseOver)
    {
        gradient = juce::ColourGradient(
            juce::Colour(0xFF5BA1F3),
            bounds.getX(), bounds.getY(),
            juce::Colour(0xFF4689D6),
            bounds.getX(), bounds.getBottom(),
            false);
    }

    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds.toFloat(), 10.0f);

    // Button text
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText(text, bounds, juce::Justification::centred, true);
}

void StartScreenComponent::resized()
{
    auto bounds = getLocalBounds();
    auto buttonArea = bounds.withTrimmedTop(120);  // Space for logo
    auto leftHalf = buttonArea.removeFromLeft(buttonArea.getWidth() / 2);
    
    // Button size and positioning
    auto buttonWidth = 180;
    auto buttonHeight = 60;
    
    recordButton.setBounds(leftHalf.getCentreX() - buttonWidth/2,
                          leftHalf.getCentreY() - buttonHeight/2,
                          buttonWidth, buttonHeight);
    
    uploadButton.setBounds(buttonArea.getCentreX() - buttonWidth/2,
                          buttonArea.getCentreY() - buttonHeight/2,
                          buttonWidth, buttonHeight);
}

// Add these mouse event handlers
void StartScreenComponent::mouseEnter(const juce::MouseEvent& event)
{
    if (event.eventComponent == &recordButton)
        recordButtonMouseOver = true;
    else if (event.eventComponent == &uploadButton)
        uploadButtonMouseOver = true;
    repaint();
}

void StartScreenComponent::mouseExit(const juce::MouseEvent& event)
{
    if (event.eventComponent == &recordButton)
        recordButtonMouseOver = false;
    else if (event.eventComponent == &uploadButton)
        uploadButtonMouseOver = false;
    repaint();
} 