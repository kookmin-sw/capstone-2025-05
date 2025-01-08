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

    recordButton.addMouseListener(this, false);
    uploadButton.addMouseListener(this, false);

    // Set up fonts
    logoFont.setHeight(72.0f);
    
    // Load Roboto font
    auto robotoFont = juce::Typeface::createSystemTypefaceFor(BinaryData::RobotoRegular_ttf,
                                                             BinaryData::RobotoRegular_ttfSize);
    materialFont = juce::Font(robotoFont);
    materialFont.setHeight(16.0f);
}

StartScreenComponent::~StartScreenComponent()
{
}

void StartScreenComponent::paint(juce::Graphics& g)
{
    // Material background color
    g.fillAll(juce::Colour(0xFF121212));  // Material Dark theme background

    // Draw MAPLE logo
    g.setFont(logoFont);
    g.setColour(juce::Colour(0xFFE0E0E0));  // Material light text color
    auto logoBounds = getLocalBounds().removeFromTop(120);
    g.drawText("MAPLE", logoBounds, juce::Justification::centred, true);

    // Draw material buttons
    auto buttonArea = getLocalBounds().withTrimmedTop(120);
    auto leftHalf = buttonArea.removeFromLeft(buttonArea.getWidth() / 2);
    
    drawButton(g, recordButton.getBounds(), "Record", 
               recordButtonMouseOver, recordButtonMouseDown);
    drawButton(g, uploadButton.getBounds(), "Upload", 
               uploadButtonMouseOver, uploadButtonMouseDown);
}

void StartScreenComponent::drawShadow(juce::Graphics& g, juce::Rectangle<float> bounds, float elevation)
{
    // Material Design elevation shadows
    float shadowOffset = elevation * 0.5f;
    float shadowBlur = elevation * 2.0f;
    
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds.translated(0, shadowOffset).expanded(2),
                          4.0f, shadowBlur);
}

void StartScreenComponent::drawButton(juce::Graphics& g, juce::Rectangle<int> bounds, 
                                    const juce::String& text, bool isMouseOver, bool isMouseDown)
{
    auto buttonBounds = bounds.toFloat().reduced(10);
    
    // Draw shadow
    float elevation = isMouseDown ? 2.0f : (isMouseOver ? 8.0f : 4.0f);
    drawShadow(g, buttonBounds, elevation);

    // Button color from Material Design palette
    juce::Colour buttonColor = juce::Colour(0xFF6200EE);  // Material Purple 500
    if (isMouseOver)
        buttonColor = buttonColor.brighter(0.2f);
    if (isMouseDown)
        buttonColor = buttonColor.darker(0.2f);

    // Draw button background
    g.setColour(buttonColor);
    g.fillRoundedRectangle(buttonBounds, 4.0f);

    // Draw text
    g.setColour(juce::Colours::white);
    g.setFont(materialFont.withHeight(14.0f));
    g.drawText(text.toUpperCase(), buttonBounds, juce::Justification::centred, true);
}

void StartScreenComponent::resized()
{
    auto bounds = getLocalBounds();
    auto buttonArea = bounds.withTrimmedTop(120);
    auto leftHalf = buttonArea.removeFromLeft(buttonArea.getWidth() / 2);
    
    // Material Design recommended touch target size (48dp)
    auto buttonWidth = 160;
    auto buttonHeight = 48;
    
    recordButton.setBounds(leftHalf.getCentreX() - buttonWidth/2,
                          leftHalf.getCentreY() - buttonHeight/2,
                          buttonWidth, buttonHeight);
    
    uploadButton.setBounds(buttonArea.getCentreX() - buttonWidth/2,
                          buttonArea.getCentreY() - buttonHeight/2,
                          buttonWidth, buttonHeight);
}

void StartScreenComponent::mouseDown(const juce::MouseEvent& event)
{
    if (event.eventComponent == &recordButton)
        recordButtonMouseDown = true;
    else if (event.eventComponent == &uploadButton)
        uploadButtonMouseDown = true;
    repaint();
}

void StartScreenComponent::mouseUp(const juce::MouseEvent& event)
{
    recordButtonMouseDown = false;
    uploadButtonMouseDown = false;
    repaint();
}

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