#pragma once

#include <JuceHeader.h>
#include "../Models/ApplicationState.h"

class ApplicationController
{
public:
    ApplicationController();
    
    void createNewProject();
    void closeProject();
    bool isProjectOpen() const;
    
    std::function<void()> onProjectStateChanged;

private:
    std::unique_ptr<ApplicationState> state;
}; 