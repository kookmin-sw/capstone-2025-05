#pragma once
#include <JuceHeader.h>
#include "AlbumGridView.h"
#include "Model/ContentModel.h"

class MainComponent;  // 전방 선언
class ContentController; // 전방 선언

// ContentPanelComponent - MVC 패턴의 View 역할
class ContentPanelComponent : public juce::Component
{
public:
    ContentPanelComponent();  // 생성자 수정
    ~ContentPanelComponent() = default;
    
    void resized() override;
    
    // 컨트롤러 설정
    void setContentController(ContentController* controller);
    
    // 화면 초기화 (API 데이터 로드)
    void initialize();
    
    // 곡 선택 이벤트 리스너 인터페이스
    class SongSelectedListener
    {
    public:
        virtual ~SongSelectedListener() = default;
        virtual void songSelected(const juce::String& songId) = 0;
    };
    
    void addSongSelectedListener(SongSelectedListener* listener) { songListeners.add(listener); }
    void removeSongSelectedListener(SongSelectedListener* listener) { songListeners.remove(listener); }

private:
    // 곡 선택 이벤트를 리스너에게 통지
    void notifySongSelected(const juce::String& songId);
    
    // API로부터 데이터 로드
    void loadDataFromAPI();
    
    // 곡 데이터 화면에 표시
    void updateGridViews();

    // UI 컴포넌트
    juce::Label recentTitle;
    AlbumGridView recentGrid;
    juce::TextButton viewAllRecent;

    juce::Label recommendTitle;
    AlbumGridView recommendGrid;
    juce::TextButton viewAllRecommend;
    
    // 로딩 인디케이터
    juce::Label loadingLabel;
    bool isLoading = false;
    
    // MVC 컴포넌트
    ContentController* contentController = nullptr;

    // 리스너 리스트
    juce::ThreadSafeListenerList<SongSelectedListener> songListeners;
};