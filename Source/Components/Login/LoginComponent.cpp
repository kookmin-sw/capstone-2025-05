#include "LoginComponent.h"

LoginComponent::LoginComponent(std::function<void()> loginCallback)
    : onLoginSuccess(std::move(loginCallback))
{
    initializeComponents();
    setupLayout();
}

LoginComponent::~LoginComponent()
{
}

void LoginComponent::paint(juce::Graphics& g)
{
    // 배경 그리기
    g.fillAll(backgroundColour);
    
    // 오른쪽 패널 배경
    auto bounds = getLocalBounds();
    auto rightPanel = bounds.removeFromRight(bounds.getWidth() / 2);
    g.setColour(rightPanelColour);
    g.fillRect(rightPanel);

    // 입력 필드 밑줄 그리기
    auto rightContent = rightPanel.reduced(40);
    rightContent.removeFromTop(40); // 로그인 타이틀 공간
    rightContent.removeFromTop(40); // 간격
    rightContent.removeFromTop(30); // Customization Settings 라벨 공간

    // 기타 디테일 입력 필드 밑줄
    auto inputBounds = rightContent.removeFromTop(40);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawLine(inputBounds.getX(), inputBounds.getBottom(),
               inputBounds.getRight(), inputBounds.getBottom(), 1.0f);

    rightContent.removeFromTop(20); // 간격
    rightContent.removeFromTop(30); // 패스워드 라벨 공간

    // 패스워드 입력 필드 밑줄
    inputBounds = rightContent.removeFromTop(40);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawLine(inputBounds.getX(), inputBounds.getBottom(),
               inputBounds.getRight(), inputBounds.getBottom(), 1.0f);
}

void LoginComponent::resized()
{
    auto bounds = getLocalBounds();
    auto leftPanel = bounds.removeFromLeft(bounds.getWidth() / 2);
    auto rightPanel = bounds;

    // 왼쪽 패널 레이아웃
    const int padding = 40;
    auto leftContent = leftPanel.reduced(padding);
    
    if (logoImage)
    {
        logoImage->setBounds(leftContent.removeFromTop(300));
    }
    
    leftContent.removeFromTop(20); // 간격
    
    if (titleLabel)
    {
        titleLabel->setBounds(leftContent.removeFromTop(60));
    }
    
    if (descriptionLabel)
    {
        descriptionLabel->setBounds(leftContent.removeFromTop(80));
    }

    // 오른쪽 패널 레이아웃
    auto rightContent = rightPanel.reduced(padding);
    
    if (loginTitleLabel)
    {
        loginTitleLabel->setBounds(rightContent.removeFromTop(40));
    }
    
    rightContent.removeFromTop(40); // 간격
    
    if (customizationLabel)
    {
        customizationLabel->setBounds(rightContent.removeFromTop(30));
    }
    
    if (guitarDetailsInput)
    {
        guitarDetailsInput->setBounds(rightContent.removeFromTop(40));
    }
    
    rightContent.removeFromTop(20); // 간격
    
    if (passwordLabel)
    {
        passwordLabel->setBounds(rightContent.removeFromTop(30));
    }
    
    if (passwordInput)
    {
        passwordInput->setBounds(rightContent.removeFromTop(40));
    }
    
    rightContent.removeFromTop(30); // 간격
    
    if (loginButton)
    {
        loginButton->setBounds(rightContent.removeFromTop(45));
    }
    
    rightContent.removeFromTop(20); // 간격
    
    if (forgotPasswordLink)
    {
        forgotPasswordLink->setBounds(rightContent.removeFromTop(30));
    }
}

void LoginComponent::initializeComponents()
{
    // 왼쪽 패널 초기화
    logoImage.reset(new juce::ImageComponent());
    addAndMakeVisible(logoImage.get());
    // 이미지 로드는 리소스 파일에서 해야 합니다
    
    titleLabel.reset(new juce::Label());
    addAndMakeVisible(titleLabel.get());
    titleLabel->setText("GuitarPro offers real-time analysis tools.", juce::dontSendNotification);
    titleLabel->setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel->setJustificationType(juce::Justification::centred);
    
    descriptionLabel.reset(new juce::Label());
    addAndMakeVisible(descriptionLabel.get());
    descriptionLabel->setText("Enhance your guitar skills with MAPLE: a cutting-edge platform for real-time music", juce::dontSendNotification);
    descriptionLabel->setFont(juce::Font(16.0f));
    descriptionLabel->setJustificationType(juce::Justification::centred);

    // 오른쪽 패널 초기화
    loginTitleLabel.reset(new juce::Label());
    addAndMakeVisible(loginTitleLabel.get());
    loginTitleLabel->setText("Log in", juce::dontSendNotification);
    loginTitleLabel->setFont(juce::Font(24.0f, juce::Font::bold));
    
    customizationLabel.reset(new juce::Label());
    addAndMakeVisible(customizationLabel.get());
    customizationLabel->setText("Customization Settings", juce::dontSendNotification);
    customizationLabel->setFont(juce::Font(16.0f));
    
    guitarDetailsInput.reset(new juce::TextEditor());
    addAndMakeVisible(guitarDetailsInput.get());
    guitarDetailsInput->setTextToShowWhenEmpty("Enter your guitar details", juce::Colours::grey);
    guitarDetailsInput->setFont(juce::Font(16.0f));
    guitarDetailsInput->setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    guitarDetailsInput->setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    guitarDetailsInput->setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
    guitarDetailsInput->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    guitarDetailsInput->setColour(juce::CaretComponent::caretColourId, juce::Colours::black);
    
    passwordLabel.reset(new juce::Label());
    addAndMakeVisible(passwordLabel.get());
    passwordLabel->setText("Password", juce::dontSendNotification);
    passwordLabel->setFont(juce::Font(16.0f));
    
    passwordInput.reset(new juce::TextEditor());
    addAndMakeVisible(passwordInput.get());
    passwordInput->setPasswordCharacter('•');
    passwordInput->setFont(juce::Font(16.0f));
    passwordInput->setTextToShowWhenEmpty("Enter your password", juce::Colours::grey);
    passwordInput->setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    passwordInput->setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    passwordInput->setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
    passwordInput->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    passwordInput->setColour(juce::CaretComponent::caretColourId, juce::Colours::black);
    
    loginButton.reset(new juce::TextButton("Log in"));
    addAndMakeVisible(loginButton.get());
    loginButton->setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFD3C1B9));
    loginButton->onClick = [this]() { onLoginSuccess(); };
    
    forgotPasswordLink.reset(new juce::HyperlinkButton("Forgot your password?", juce::URL("https://example.com/forgot-password")));
    addAndMakeVisible(forgotPasswordLink.get());
    forgotPasswordLink->setFont(juce::Font(14.0f), false);
}

void LoginComponent::setupLayout()
{
    setSize(900, 600);
} 