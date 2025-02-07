#include "LoginComponent.h"
#include "../../Services/AuthService.h"

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
    g.fillAll(MapleColours::currentTheme.background);
    
    // 오른쪽 패널 배경
    auto bounds = getLocalBounds();
    auto rightPanel = bounds.removeFromRight(bounds.getWidth() / 2);
    g.setColour(MapleColours::currentTheme.panel);
    g.fillRect(rightPanel);

    // 입력 필드 밑줄 그리기
    auto rightContent = rightPanel.reduced(40);
    rightContent.removeFromTop(40); // 로그인 타이틀 공간
    rightContent.removeFromTop(40); // 간격
    rightContent.removeFromTop(30); // Customization Settings 라벨 공간

    // 기타 디테일 입력 필드 밑줄
    auto inputBounds = rightContent.removeFromTop(40);
    g.setColour(MapleColours::currentTheme.border);
    g.drawLine(inputBounds.getX(), inputBounds.getBottom(),
               inputBounds.getRight(), inputBounds.getBottom(), 1.0f);

    rightContent.removeFromTop(20); // 간격
    rightContent.removeFromTop(30); // 패스워드 라벨 공간

    // 패스워드 입력 필드 밑줄
    inputBounds = rightContent.removeFromTop(40);
    g.setColour(MapleColours::currentTheme.border);
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

    // 에러 메시지 레이아웃
    if (errorLabel != nullptr)
    {
        rightContent.removeFromTop(40); // 로그인 타이틀 공간
        rightContent.removeFromTop(40); // 간격
        
        // 에러 메시지를 로그인 버튼 위에 배치
        auto errorBounds = rightContent;
        errorBounds.setHeight(20);
        errorBounds.translate(0, -25);
        errorLabel->setBounds(errorBounds);
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
    titleLabel->setText(juce::String::fromUTF8(u8"GuitarPro offers real-time analysis tools."), juce::dontSendNotification);
    titleLabel->setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel->setJustificationType(juce::Justification::centred);

    titleLabel->setColour(juce::Label::textColourId, MapleColours::currentTheme.buttonText);
    
    descriptionLabel.reset(new juce::Label());
    addAndMakeVisible(descriptionLabel.get());
    descriptionLabel->setText(juce::String::fromUTF8(u8"Enhance your guitar skills with MAPLE: a cutting-edge platform for real-time music"), juce::dontSendNotification);
    descriptionLabel->setFont(juce::Font(16.0f));
    descriptionLabel->setJustificationType(juce::Justification::centred);

    descriptionLabel->setColour(juce::Label::textColourId, MapleColours::currentTheme.text);

    // 오른쪽 패널 초기화
    loginTitleLabel.reset(new juce::Label());
    addAndMakeVisible(loginTitleLabel.get());
    loginTitleLabel->setText(juce::String::fromUTF8(u8"Log in"), juce::dontSendNotification);
    loginTitleLabel->setFont(juce::Font(24.0f, juce::Font::bold));
    loginTitleLabel->setColour(juce::Label::textColourId, MapleColours::currentTheme.buttonText);
    
    customizationLabel.reset(new juce::Label());
    addAndMakeVisible(customizationLabel.get());
    customizationLabel->setText(juce::String::fromUTF8(u8"Customization Settings"), juce::dontSendNotification);
    customizationLabel->setFont(juce::Font(16.0f));
    customizationLabel->setColour(juce::Label::textColourId, MapleColours::currentTheme.text);
    
    guitarDetailsInput.reset(new MapleTextEditor());
    addAndMakeVisible(guitarDetailsInput.get());
    guitarDetailsInput->setTextToShowWhenEmpty(juce::String::fromUTF8(u8"Enter your guitar details"), juce::Colours::grey);
    guitarDetailsInput->setFont(juce::Font(16.0f));
    guitarDetailsInput->setColour(juce::TextEditor::textColourId, MapleColours::currentTheme.buttonText);
    guitarDetailsInput->setColour(juce::CaretComponent::caretColourId, MapleColours::currentTheme.buttonText);
    
    passwordLabel.reset(new juce::Label());
    addAndMakeVisible(passwordLabel.get());
    passwordLabel->setText(juce::String::fromUTF8(u8"Password"), juce::dontSendNotification);
    passwordLabel->setFont(juce::Font(16.0f));
    passwordLabel->setColour(juce::Label::textColourId, MapleColours::currentTheme.text);
    
    passwordInput.reset(new MapleTextEditor());
    addAndMakeVisible(passwordInput.get());
    passwordInput->setPasswordCharacter(juce::juce_wchar(0x2022));  // 유니코드 bullet point •
    passwordInput->setFont(juce::Font(16.0f));
    passwordInput->setTextToShowWhenEmpty(juce::String::fromUTF8(u8"Enter your password"), juce::Colours::grey);
    passwordInput->setColour(juce::TextEditor::textColourId, MapleColours::currentTheme.buttonText);

    passwordInput->setColour(juce::CaretComponent::caretColourId, MapleColours::currentTheme.buttonText);
    
    loginButton.reset(new juce::TextButton(juce::String::fromUTF8(u8"Log in")));
    addAndMakeVisible(loginButton.get());
    loginButton->setColour(juce::TextButton::buttonColourId, MapleColours::currentTheme.buttonNormal);
    loginButton->setColour(juce::TextButton::textColourOffId, MapleColours::currentTheme.buttonText);
    loginButton->onClick = [this]() { handleLoginAttempt(); };
    
    forgotPasswordLink.reset(new juce::HyperlinkButton(juce::String::fromUTF8(u8"Forgot your password?"), juce::URL("https://example.com/forgot-password")));
    addAndMakeVisible(forgotPasswordLink.get());
    forgotPasswordLink->setFont(juce::Font(14.0f), false);
    forgotPasswordLink->setColour(juce::HyperlinkButton::textColourId, MapleColours::currentTheme.buttonNormal);
}

void LoginComponent::setupLayout()
{
    setSize(900, 600);
}

void LoginComponent::handleLoginAttempt()
{
    // 입력값 가져오기
    juce::String username = guitarDetailsInput->getText();
    juce::String password = passwordInput->getText();

    // 기본적인 유효성 검사
    if (username.isEmpty() || password.isEmpty())
    {
        showErrorMessage(juce::String::fromUTF8(u8"Please fill in all fields"));
        return;
    }

    // UI 업데이트는 현재 스레드(메인 스레드)에서 수행
    loginButton->setEnabled(false);
    loginButton->setButtonText(juce::String::fromUTF8(u8"Logging in..."));

    // 네트워크 요청과 응답 처리를 별도 스레드에서 수행
    std::thread([this, username, password]()
    {
        AuthService::LoginRequest request{ username, password };
        auto response = AuthService::login(request);

        // UI 업데이트는 다시 메인 스레드에서
        juce::MessageManager::callAsync([this, response]()
        {
            if (response.success)
            {
                onLoginSuccess();
            }
            else
            {
                showErrorMessage(response.message);
                loginButton->setEnabled(true);
                loginButton->setButtonText(juce::String::fromUTF8(u8"Log in"));
            }
        });
    }).detach();
}

void LoginComponent::showErrorMessage(const juce::String& message)
{
    if (errorLabel == nullptr)
    {
        errorLabel.reset(new juce::Label());
        addAndMakeVisible(errorLabel.get());
        errorLabel->setColour(juce::Label::textColourId, juce::Colours::red);
        errorLabel->setFont(juce::Font(14.0f));
        errorLabel->setJustificationType(juce::Justification::centred);
    }

    errorLabel->setText(message, juce::dontSendNotification);
    resized();  // 레이아웃 업데이트
} 