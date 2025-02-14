#pragma once
#include <JuceHeader.h>

class ApplicationState
{
public:
    ApplicationState() : isProjectActive(false) {}
    
    bool isProjectActive;
    juce::String currentProjectPath;
    
    void setProjectActive(bool active, const juce::String& path = {})
    {
        isProjectActive = active;
        currentProjectPath = path;
    }
}; 