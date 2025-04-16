#include "ContentController.h"
#include "../MainComponent.h"
#include "../PracticeSongComponent.h"
#include "../ContentPanelComponent.h"

// Controller가 Model 객체 참조를 가져오도록 수정
ContentController::ContentController(ContentModel& model, MainComponent& mainComp, 
                                    PracticeSongComponent& practiceSongComp)
    : contentModel(model), 
      mainComponent(mainComp),
      practiceSongComponent(practiceSongComp)
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

// songSelected 메서드 구현 - MainComponent에서 이동
void ContentController::songSelected(const juce::String& songId)
{
    DBG("ContentController: Song selected with ID: " + songId);
    
    // 선택된 곡 ID 저장
    selectedSongId = songId;
    
    // 곡 선택 후 연습 화면으로 전환
    mainComponent.showPracticeScreen();
    
    // PracticeSongComponent에 선택된 곡 로드 요청
    bool loadSuccess = practiceSongComponent.loadSong(songId);
    
    if (!loadSuccess)
    {
        // 로드 실패 시 메인 화면으로 돌아가기
        DBG("Failed to load song: " + songId);
        mainComponent.showMainScreen();
    }
}