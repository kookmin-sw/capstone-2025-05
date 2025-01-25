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
    auto iconBounds = bounds.removeFromLeft(bounds.getHeight()).reduced(2);
    
    // Draw background if mouse is over
    if (isMouseOver)
    {
        g.setColour(juce::Colour(0xFF2A2A2A));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
    }
    
    // Draw icon
    if (icon.isValid())
    {
        g.drawImage(icon, iconBounds.toFloat(), juce::RectanglePlacement::centred);
    }
    
    // Draw text
    g.setColour(isMouseOver ? juce::Colours::white : juce::Colour(0xFFAAAAAA));
    g.setFont(16.0f);
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