#include "ContentPanelComponent.h"

ContentPanelComponent::ContentPanelComponent()
{
    // 좌측 패널 설정
    recentTitle.setText(juce::String::fromUTF8("최근 연습 목록 (RECENT)"), 
                       juce::dontSendNotification);
    recentList.setModel(&recentListModel);
    viewAllRecent.setButtonText(juce::String::fromUTF8("모두 보기"));
    
    addAndMakeVisible(recentTitle);
    addAndMakeVisible(recentList);
    addAndMakeVisible(viewAllRecent);

    // 우측 패널 설정
    recommendTitle.setText(juce::String::fromUTF8("추천 곡 (RECOMMENDED)"), 
                         juce::dontSendNotification);
    recommendList.setModel(&recommendListModel);
    viewAllRecommend.setButtonText(juce::String::fromUTF8("모두 보기"));
    
    addAndMakeVisible(recommendTitle);
    addAndMakeVisible(recommendList);
    addAndMakeVisible(viewAllRecommend);
}

void ContentPanelComponent::resized()
{
    auto bounds = getLocalBounds();
    auto leftPanel = bounds.removeFromLeft(bounds.getWidth() / 2);
    
    // 좌측 패널 레이아웃
    recentTitle.setBounds(leftPanel.removeFromTop(30));
    viewAllRecent.setBounds(leftPanel.removeFromBottom(30));
    recentList.setBounds(leftPanel);

    // 우측 패널 레이아웃
    recommendTitle.setBounds(bounds.removeFromTop(30));
    viewAllRecommend.setBounds(bounds.removeFromBottom(30));
    recommendList.setBounds(bounds);
} 