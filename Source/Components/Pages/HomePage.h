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

    // 프로젝트 시작 버튼
    std::unique_ptr<MapleButton> createProjectBtn;

    juce::TextEditor searchInput;
    juce::TextButton searchButton;
    juce::Array<SpotifyService::Album> albums;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomePage)
};