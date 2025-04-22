#include "ContentController.h"
#include "MainComponent.h"
#include "View/GuitarPracticeComponent.h"
#include "View/ContentPanelComponent.h"

// Controller가 Model 객체 참조를 가져오도록 수정
ContentController::ContentController(ContentModel& model, MainComponent& mainComp, 
                                    GuitarPracticeComponent& practiceSongComp)
    : contentModel(model), 
      mainComponent(mainComp),
      guitarPracticeComponent(practiceSongComp)
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
    
    // PracticeSongComponent에 선택된 곡 로드 요청 (void 타입이므로 반환값 확인 불가)
    guitarPracticeComponent.loadSong(songId);
    
    // 참고: loadSong이 bool 반환 타입이었으나 void로 변경되어 성공/실패 확인 로직은 제거됨
    // 향후 View에서 로드 실패 시 Controller로 알려주는 콜백 또는 이벤트 구현 필요
}