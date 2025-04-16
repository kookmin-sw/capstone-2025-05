#pragma once
#include <JuceHeader.h>
#include "../Model/ContentModel.h"

// ContentController 클래스 - 컨텐츠 관련 비즈니스 로직을 처리합니다
class ContentController
{
public:
    ContentController(ContentModel& model);
    ~ContentController() = default;
    
    // 곡 데이터 초기화
    void initializeData();
    
    // 최근 곡 관련 처리
    const juce::Array<Song>& getRecentSongs() const;
    
    // 추천 곡 관련 처리
    const juce::Array<Song>& getRecommendedSongs() const;
    
private:
    ContentModel& contentModel;
};