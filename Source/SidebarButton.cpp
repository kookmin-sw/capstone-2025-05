#include "SidebarButton.h"

SidebarButton::SidebarButton(const juce::String& buttonName, const juce::String& iconPath)
    : name(buttonName)
{
    auto file = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                    .getParentDirectory()
                    .getParentDirectory()
                    .getParentDirectory()
                    .getParentDirectory()
                    .getParentDirectory()
                    .getParentDirectory()
                    .getChildFile(iconPath);
                    
    if (file.existsAsFile())
    {
        icon = juce::ImageFileFormat::loadFrom(file);
    }

    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void SidebarButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(5);
    auto iconBounds = bounds.removeFromLeft(25).reduced(2);
    
    // Draw background if mouse is over
    if (isMouseOver)
    {
        g.setColour(juce::Colour(0xFFE6D5C5));  // Lighter mocha for hover
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
    }
    
    // Draw icon
    if (icon.isValid())
    {
        g.drawImage(icon, iconBounds.toFloat(), juce::RectanglePlacement::centred);
    }

    if(name == "MAPLE")
    {
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(20.0f).boldened());
    }
    else
    {
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(14.0f).boldened());
    }

    bounds.removeFromLeft(5);
    g.drawText(name, bounds, juce::Justification::centredLeft);
}

void SidebarButton::mouseEnter(const juce::MouseEvent&)
{
    isMouseOver = true;
    repaint();
}

void SidebarButton::mouseExit(const juce::MouseEvent&)
{
    isMouseOver = false;
    repaint();
} 