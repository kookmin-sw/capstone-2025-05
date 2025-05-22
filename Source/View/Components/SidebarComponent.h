#pragma once
#include <JuceHeader.h>
#include "../LookAndFeel/MapleTheme.h"

/**
 * Spotify 스타일의 사이드바 컴포넌트
 * 메뉴 항목과 네비게이션을 제공합니다.
 */
class SidebarComponent : public juce::Component,
                         public juce::Button::Listener
{
public:
    // 사이드바 항목 타입
    enum class ItemType {
        Home,
        Library,
        Practice,
        Settings,
        History,
        Favorites
    };
    
    SidebarComponent()
    {
        // 사이드바 제목
        titleLabel.setText("Maple", juce::dontSendNotification);
        titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
        titleLabel.setColour(juce::Label::textColourId, MapleTheme::getTextColour());
        addAndMakeVisible(titleLabel);
        
        // 메뉴 버튼 초기화
        addMenuItem(homeButton, "Home", ItemType::Home);
        addMenuItem(libraryButton, "My Library", ItemType::Library);
        addMenuItem(practiceButton, "Practice", ItemType::Practice);
        
        // 구분선 (Separator)
        addAndMakeVisible(separator1);
        
        // 추가 메뉴 항목
        addMenuItem(favoritesButton, "Favorites", ItemType::Favorites);
        addMenuItem(historyButton, "History", ItemType::History);
        addMenuItem(settingsButton, "Settings", ItemType::Settings);
        
        // 첫 번째 항목 선택
        setSelectedMenuItem(ItemType::Home);
    }
    
    ~SidebarComponent() override
    {
        homeButton.removeListener(this);
        libraryButton.removeListener(this);
        practiceButton.removeListener(this);
        favoritesButton.removeListener(this);
        historyButton.removeListener(this);
        settingsButton.removeListener(this);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleTheme::getSidebarColour());
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        
        // 제목 영역
        titleLabel.setBounds(bounds.removeFromTop(50));
        
        // 메인 메뉴 영역
        bounds.removeFromTop(10); // 간격
        homeButton.setBounds(bounds.removeFromTop(40));
        bounds.removeFromTop(5);
        libraryButton.setBounds(bounds.removeFromTop(40));
        bounds.removeFromTop(5);
        practiceButton.setBounds(bounds.removeFromTop(40));
        
        // 구분선
        bounds.removeFromTop(20);
        separator1.setBounds(bounds.removeFromTop(1).withSizeKeepingCentre(bounds.getWidth() - 20, 1));
        bounds.removeFromTop(20);
        
        // 추가 메뉴 영역
        favoritesButton.setBounds(bounds.removeFromTop(40));
        bounds.removeFromTop(5);
        historyButton.setBounds(bounds.removeFromTop(40));
        
        // 설정 버튼은 하단에 배치
        settingsButton.setBounds(getLocalBounds().removeFromBottom(60).reduced(10));
    }
    
    void buttonClicked(juce::Button* button) override
    {
        // 클릭된 버튼에 따라 선택된 메뉴 항목 업데이트
        if (button == &homeButton)
            setSelectedMenuItem(ItemType::Home);
        else if (button == &libraryButton)
            setSelectedMenuItem(ItemType::Library);
        else if (button == &practiceButton)
            setSelectedMenuItem(ItemType::Practice);
        else if (button == &favoritesButton)
            setSelectedMenuItem(ItemType::Favorites);
        else if (button == &historyButton)
            setSelectedMenuItem(ItemType::History);
        else if (button == &settingsButton)
            setSelectedMenuItem(ItemType::Settings);
            
        // 콜백 호출
        if (onMenuItemSelected)
            onMenuItemSelected(selectedItem);
    }
    
    // 메뉴 항목 선택 콜백 설정
    void setOnMenuItemSelectedCallback(std::function<void(ItemType)> callback)
    {
        onMenuItemSelected = std::move(callback);
    }
    
    // 현재 선택된 메뉴 항목 설정
    void setSelectedMenuItem(ItemType type)
    {
        selectedItem = type;
        
        // 모든 버튼의 선택 상태 초기화
        homeButton.setToggleState(type == ItemType::Home, juce::dontSendNotification);
        libraryButton.setToggleState(type == ItemType::Library, juce::dontSendNotification);
        practiceButton.setToggleState(type == ItemType::Practice, juce::dontSendNotification);
        favoritesButton.setToggleState(type == ItemType::Favorites, juce::dontSendNotification);
        historyButton.setToggleState(type == ItemType::History, juce::dontSendNotification);
        settingsButton.setToggleState(type == ItemType::Settings, juce::dontSendNotification);
        
        repaint();
    }
    
private:
    // 사이드바 메뉴 항목 추가 헬퍼 메서드
    void addMenuItem(juce::TextButton& button, const juce::String& text, ItemType type)
    {
        button.setButtonText(text);
        button.setClickingTogglesState(true);
        button.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
        button.setColour(juce::TextButton::textColourOffId, MapleTheme::getSubTextColour());
        button.setColour(juce::TextButton::textColourOnId, MapleTheme::getTextColour());
        button.addListener(this);
        addAndMakeVisible(button);
    }
    
    juce::Label titleLabel;
    juce::TextButton homeButton;
    juce::TextButton libraryButton;
    juce::TextButton practiceButton;
    juce::TextButton favoritesButton;
    juce::TextButton historyButton;
    juce::TextButton settingsButton;
    
    juce::Component separator1;
    
    ItemType selectedItem = ItemType::Home;
    std::function<void(ItemType)> onMenuItemSelected;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarComponent)
}; 