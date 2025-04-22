#pragma once
#include <JuceHeader.h>
#include "View/AlbumGridView.h"
#include "Model/ContentModel.h"

class MainComponent;  // 전방 선언
class ContentController; // 전방 선언

// ContentPanelComponent - MVC 패턴의 View 역할
class ContentPanelComponent : public juce::Component
{
public:
    ContentPanelComponent();  // 생성자 수정
    void resized() override;
    
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

    // UI 컴포넌트
    juce::Label recentTitle;
    AlbumGridView recentGrid;
    juce::TextButton viewAllRecent;

    juce::Label recommendTitle;
    AlbumGridView recommendGrid;
    juce::TextButton viewAllRecommend;
    
    // MVC 컴포넌트
    ContentModel contentModel;

    // 리스너 리스트
    juce::ListenerList<SongSelectedListener> songListeners;
};