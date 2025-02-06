#pragma once

#include <JuceHeader.h>

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
    std::unique_ptr<juce::TextEditor> guitarDetailsInput;
    std::unique_ptr<juce::Label> passwordLabel;
    std::unique_ptr<juce::TextEditor> passwordInput;
    std::unique_ptr<juce::TextButton> loginButton;
    std::unique_ptr<juce::HyperlinkButton> forgotPasswordLink;

    // 배경색 설정
    juce::Colour backgroundColour { juce::Colour(0xFFF5E6E6) };  // 베이지색
    juce::Colour rightPanelColour { juce::Colours::white };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoginComponent)
};
