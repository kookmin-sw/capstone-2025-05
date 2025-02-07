#pragma once

#include <JuceHeader.h>
#include "../../UI/Styles/MapleColours.h"  // 테마 헤더 추가
#include "../../UI/Common/TextEditor/MapleTextEditor.h"  // 경로 수정

class LoginComponent : public juce::Component
{
public:
    LoginComponent(std::function<void()> loginCallback);
    ~LoginComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void initializeComponents();
    void setupLayout();
    
    // 콜백 함수 저장
    std::function<void()> onLoginSuccess;
    
    // 왼쪽 패널 컴포넌트들
    std::unique_ptr<juce::ImageComponent> logoImage;
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::Label> descriptionLabel;
    
    // 오른쪽 패널 컴포넌트들
    std::unique_ptr<juce::Label> loginTitleLabel;
    std::unique_ptr<juce::Label> customizationLabel;
    std::unique_ptr<MapleTextEditor> guitarDetailsInput;
    std::unique_ptr<juce::Label> passwordLabel;
    std::unique_ptr<MapleTextEditor> passwordInput;
    std::unique_ptr<juce::TextButton> loginButton;
    std::unique_ptr<juce::HyperlinkButton> forgotPasswordLink;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoginComponent)
};
