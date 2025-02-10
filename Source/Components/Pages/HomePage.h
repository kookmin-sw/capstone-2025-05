#pragma once
#include "Page.h"
#include "../../UI/Common/Button/MapleButton.h"
#include "../../UI/Styles/MapleTypography.h"
#include "../../Services/SpotifyService.h"

class HomePage : public Page
{
public:
    HomePage();
    ~HomePage() override;

    juce::String getPageName() const override { return "Home"; }
    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    void drawSection(juce::Graphics &g, const juce::String &title,
                     juce::Rectangle<int> bounds, bool showMoreButton = true);
    void drawSectionHeader(juce::Graphics &g, const juce::String &title,
                           juce::Rectangle<int> bounds, bool showMoreButton);
    void drawRecommendedSection(juce::Graphics& g, juce::Rectangle<int> bounds);

    void createProjectButton();
    void searchAlbums(const juce::String& query);
    void displayAlbums();

    void drawAlbumCover(juce::Graphics& g, const SpotifyService::Album& album, int x, int y);
    void loadAlbumCovers();

    class FadeTimer : public juce::Timer
    {
    public:
        FadeTimer(HomePage& parent) : owner(parent) {}
        void timerCallback() override 
        {
            owner.updateFadeAnimation();
        }
    private:
        HomePage& owner;
    };

    // AlbumContainer 클래스 추가
    class AlbumContainer : public juce::Component
    {
    public:
        AlbumContainer(HomePage& parent) : owner(parent) {}
        
        void paint(juce::Graphics& g) override
        {
            owner.paintAlbumContainer(g);
        }
        
    private:
        HomePage& owner;
    };

    // 멤버 변수들
    std::unique_ptr<MapleButton> createProjectBtn;
    std::unique_ptr<MapleButton> prevAlbumBtn;
    std::unique_ptr<MapleButton> nextAlbumBtn;
    std::vector<SpotifyService::Album> albums;
    std::unique_ptr<FadeTimer> fadeAnimator;
    int currentAlbumPage = 0;
    juce::TextEditor searchInput;
    juce::TextButton searchButton;
    float imageAlpha = 0.0f;
    std::unique_ptr<juce::Viewport> albumViewport;
    std::unique_ptr<AlbumContainer> albumContainer;
    std::unique_ptr<MapleButton> scrollLeftBtn;
    std::unique_ptr<MapleButton> scrollRightBtn;

    void updateFadeAnimation();
    void loadAlbumCover(int index);
    void paintAlbumContainer(juce::Graphics& g);
    void updateAlbumNavButtons();
    void scrollAlbums(bool scrollRight);

    class ScrollAnimator : public juce::Timer
    {
    public:
        ScrollAnimator(HomePage& parent) : owner(parent) {}
        
        void startScrolling(int targetX)
        {
            startPosition = owner.albumViewport->getViewPositionX();
            endPosition = targetX;
            currentPosition = startPosition;
            progress = 0.0f;
            startTimer(16); // 60fps
        }
        
        void timerCallback() override
        {
            progress += 0.04f; // 속도를 절반으로 줄임
            
            if (progress >= 1.0f)
            {
                owner.albumViewport->setViewPosition(endPosition, owner.albumViewport->getViewPositionY());
                stopTimer();
                return;
            }
            
            // 더 부드러운 easeInOut 적용
            float t = progress;
            float easedProgress = t < 0.5f ? 4 * t * t * t 
                                         : 1 - std::pow(-2 * t + 2, 3) / 2;
            
            currentPosition = startPosition + (endPosition - startPosition) * easedProgress;
            owner.albumViewport->setViewPosition(currentPosition, owner.albumViewport->getViewPositionY());
        }
        
    private:
        HomePage& owner;
        float progress = 0.0f;
        int startPosition = 0;
        int endPosition = 0;
        int currentPosition = 0;
    };
    
    std::unique_ptr<ScrollAnimator> scrollAnimator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomePage)
};