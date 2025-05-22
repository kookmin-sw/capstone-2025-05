#pragma once
#include <JuceHeader.h>
#include "SongCard.h"
#include "../LookAndFeel/MapleTheme.h"

/**
 * 노래 카드들을 그리드 형태로 표시하는 컴포넌트
 * Spotify의 앨범/플레이리스트 그리드와 유사한 레이아웃입니다.
 */
class SongGridView : public juce::Component
{
public:
    SongGridView(const juce::String& title = "Featured Songs")
    : sectionTitle(title)
    {
        // 섹션 제목 라벨 초기화
        titleLabel.setText(sectionTitle, juce::dontSendNotification);
        titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
        titleLabel.setColour(juce::Label::textColourId, MapleTheme::getTextColour());
        addAndMakeVisible(titleLabel);
        
        // 스크롤 영역 초기화
        viewport.setViewedComponent(&container, false);
        viewport.setScrollBarsShown(true, false);
        addAndMakeVisible(viewport);
        
        // "더 보기" 버튼 초기화
        seeMoreButton.setButtonText("See All");
        seeMoreButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        seeMoreButton.setColour(juce::TextButton::textColourOffId, MapleTheme::getAccentColour());
        addAndMakeVisible(seeMoreButton);
    }
    
    ~SongGridView() override
    {
        clearCards();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleTheme::getBackgroundColour());
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        
        // 제목 영역
        auto headerArea = bounds.removeFromTop(40);
        titleLabel.setBounds(headerArea.removeFromLeft(200));
        seeMoreButton.setBounds(headerArea.removeFromRight(100));
        
        // 스크롤 뷰 영역
        viewport.setBounds(bounds);
        
        // 카드 컨테이너 영역 조정
        updateContainerBounds();
    }
    
    // 카드 추가
    void addCard(const SongCard::SongInfo& songInfo)
    {
        auto card = std::make_unique<SongCard>(songInfo);
        card->setOnCardSelectedCallback([this](juce::String songId) {
            if (onSongSelected)
                onSongSelected(songId);
        });
        
        addAndMakeVisible(container);
        cards.push_back(std::move(card));
        container.addAndMakeVisible(cards.back().get());
        
        updateContainerBounds();
    }
    
    // 모든 카드 제거
    void clearCards()
    {
        cards.clear();
        updateContainerBounds();
    }
    
    // 곡 선택 콜백 설정
    void setOnSongSelectedCallback(std::function<void(juce::String)> callback)
    {
        onSongSelected = std::move(callback);
    }
    
    // "더 보기" 버튼 클릭 콜백 설정
    void setOnSeeMoreCallback(std::function<void()> callback)
    {
        onSeeMore = std::move(callback);
        seeMoreButton.onClick = [this]() {
            if (onSeeMore)
                onSeeMore();
        };
    }
    
private:
    void updateContainerBounds()
    {
        if (cards.empty())
            return;
            
        // 카드 크기 및 간격 계산
        constexpr int cardWidth = 180;
        constexpr int cardHeight = 240;
        constexpr int hGap = 20;
        constexpr int vGap = 30;
        
        // 한 행에 표시할 카드 수 계산
        const int cardsPerRow = juce::jmax(1, (viewport.getWidth() - 20) / (cardWidth + hGap));
        
        // 행 수 계산
        const int rows = (cards.size() + cardsPerRow - 1) / cardsPerRow;
        
        // 그리드 레이아웃에 맞춰 카드 배치
        for (int i = 0; i < cards.size(); ++i)
        {
            const int row = i / cardsPerRow;
            const int col = i % cardsPerRow;
            
            cards[i]->setBounds(col * (cardWidth + hGap), 
                               row * (cardHeight + vGap),
                               cardWidth, cardHeight);
        }
        
        // 컨테이너 크기 설정
        container.setBounds(0, 0, 
                           cardsPerRow * (cardWidth + hGap) - hGap + 20,
                           rows * (cardHeight + vGap) - vGap + 20);
    }
    
    juce::String sectionTitle;
    juce::Label titleLabel;
    juce::TextButton seeMoreButton;
    juce::Viewport viewport;
    juce::Component container;
    
    std::vector<std::unique_ptr<SongCard>> cards;
    std::function<void(juce::String)> onSongSelected;
    std::function<void()> onSeeMore;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SongGridView)
}; 