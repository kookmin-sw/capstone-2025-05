#include "ApplicationController.h"

ApplicationController::ApplicationController()
    : state(std::make_unique<ApplicationState>())
{
}

void ApplicationController::createNewProject()
{
    state->setProjectActive(true);
    if (onProjectStateChanged)
        onProjectStateChanged();
}

void ApplicationController::closeProject()
{
    state->setProjectActive(false);
    if (onProjectStateChanged)
        onProjectStateChanged();
}

bool ApplicationController::isProjectOpen() const
{
    return state->isProjectActive;
} 