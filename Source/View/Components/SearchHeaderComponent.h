#pragma once
#include <JuceHeader.h>
#include "../LookAndFeel/MapleTheme.h"

/**
 * Spotify 스타일의 상단 헤더 컴포넌트
 * 검색, 프로필, 뒤로가기 등의 기능을 제공합니다.
 */
class SearchHeaderComponent : public juce::Component,
                             public juce::TextEditor::Listener,
                             public juce::Button::Listener
{
public:
    SearchHeaderComponent()
    {
        // 뒤로가기, 앞으로가기 버튼
        backButton.setButtonText("<");
        backButton.setColour(juce::TextButton::buttonColourId, MapleTheme::getCardColour());
        backButton.setColour(juce::TextButton::textColourOffId, MapleTheme::getTextColour());
        backButton.addListener(this);
        addAndMakeVisible(backButton);
        
        forwardButton.setButtonText(">");
        forwardButton.setColour(juce::TextButton::buttonColourId, MapleTheme::getCardColour());
        forwardButton.setColour(juce::TextButton::textColourOffId, MapleTheme::getTextColour());
        forwardButton.addListener(this);
        addAndMakeVisible(forwardButton);
        
        // 검색 필드
        searchBox.setTextToShowWhenEmpty("Search for songs...", MapleTheme::getSubTextColour());
        searchBox.setColour(juce::TextEditor::backgroundColourId, MapleTheme::getCardColour());
        searchBox.setColour(juce::TextEditor::textColourId, MapleTheme::getTextColour());
        searchBox.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        searchBox.setColour(juce::CaretComponent::caretColourId, MapleTheme::getAccentColour());
        searchBox.addListener(this);
        addAndMakeVisible(searchBox);
        
        // 프로필 버튼
        profileButton.setButtonText("Profile");
        profileButton.setColour(juce::TextButton::buttonColourId, MapleTheme::getCardColour());
        profileButton.setColour(juce::TextButton::textColourOffId, MapleTheme::getTextColour());
        profileButton.addListener(this);
        addAndMakeVisible(profileButton);
    }
    
    ~SearchHeaderComponent() override
    {
        backButton.removeListener(this);
        forwardButton.removeListener(this);
        searchBox.removeListener(this);
        profileButton.removeListener(this);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleTheme::getBackgroundColour());
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        
        // 네비게이션 버튼 (왼쪽)
        auto navWidth = 40;
        backButton.setBounds(bounds.removeFromLeft(navWidth).withSizeKeepingCentre(navWidth, navWidth));
        bounds.removeFromLeft(5);
        forwardButton.setBounds(bounds.removeFromLeft(navWidth).withSizeKeepingCentre(navWidth, navWidth));
        bounds.removeFromLeft(20);
        
        // 프로필 버튼 (오른쪽)
        profileButton.setBounds(bounds.removeFromRight(100).withSizeKeepingCentre(100, 40));
        bounds.removeFromRight(20);
        
        // 검색창 (중앙)
        searchBox.setBounds(bounds.withSizeKeepingCentre(bounds.getWidth(), 40));
    }
    
    // TextEditor::Listener 인터페이스 구현
    void textEditorTextChanged(juce::TextEditor& editor) override
    {
        if (&editor == &searchBox)
        {
            // 검색어 변경 이벤트 처리
            if (onSearchQueryChanged)
                onSearchQueryChanged(searchBox.getText());
        }
    }
    
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override
    {
        if (&editor == &searchBox)
        {
            // 엔터 키 이벤트 처리
            if (onSearchSubmitted)
                onSearchSubmitted(searchBox.getText());
        }
    }
    
    void textEditorEscapeKeyPressed(juce::TextEditor&) override {}
    void textEditorFocusLost(juce::TextEditor&) override {}
    
    // Button::Listener 인터페이스 구현
    void buttonClicked(juce::Button* button) override
    {
        if (button == &backButton)
        {
            if (onBackButtonClicked)
                onBackButtonClicked();
        }
        else if (button == &forwardButton)
        {
            if (onForwardButtonClicked)
                onForwardButtonClicked();
        }
        else if (button == &profileButton)
        {
            if (onProfileButtonClicked)
                onProfileButtonClicked();
        }
    }
    
    // 검색 콜백 설정
    void setOnSearchQueryChangedCallback(std::function<void(juce::String)> callback)
    {
        onSearchQueryChanged = std::move(callback);
    }
    
    void setOnSearchSubmittedCallback(std::function<void(juce::String)> callback)
    {
        onSearchSubmitted = std::move(callback);
    }
    
    // 네비게이션 콜백 설정
    void setOnBackButtonClickedCallback(std::function<void()> callback)
    {
        onBackButtonClicked = std::move(callback);
    }
    
    void setOnForwardButtonClickedCallback(std::function<void()> callback)
    {
        onForwardButtonClicked = std::move(callback);
    }
    
    // 프로필 콜백 설정
    void setOnProfileButtonClickedCallback(std::function<void()> callback)
    {
        onProfileButtonClicked = std::move(callback);
    }
    
private:
    juce::TextButton backButton;
    juce::TextButton forwardButton;
    juce::TextEditor searchBox;
    juce::TextButton profileButton;
    
    std::function<void(juce::String)> onSearchQueryChanged;
    std::function<void(juce::String)> onSearchSubmitted;
    std::function<void()> onBackButtonClicked;
    std::function<void()> onForwardButtonClicked;
    std::function<void()> onProfileButtonClicked;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SearchHeaderComponent)
}; 