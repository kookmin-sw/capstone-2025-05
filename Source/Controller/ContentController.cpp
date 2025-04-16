#include "ContentController.h"

ContentController::ContentController(ContentModel& model)
    : contentModel(model)
{
}

void ContentController::initializeData()
{
    contentModel.initializeSampleData();
}

const juce::Array<Song>& ContentController::getRecentSongs() const
{
    return contentModel.getRecentSongs();
}

const juce::Array<Song>& ContentController::getRecommendedSongs() const
{
    return contentModel.getRecommendedSongs();
}