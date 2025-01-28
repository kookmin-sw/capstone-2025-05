#include "StartScreenComponent.h"

StartScreenComponent::StartScreenComponent(std::function<void(Screen)> callback)
    : screenChangeCallback(std::move(callback))
{
    // Set up fonts
    auto options = juce::Font::getDefaultSansSerifFontName();
    titleFont = juce::Font(options, 72.0f, juce::Font::plain);
    descriptionFont = juce::Font(options, 20.0f, juce::Font::plain);

    // Enable mouse events
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addMouseListener(this, true);

    // Load logo image
    auto logoFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getParentDirectory()
                        .getChildFile("Resources/Images/start-screen.png");

    if (logoFile.existsAsFile())
    {
        logoImage = juce::ImageFileFormat::loadFrom(logoFile);
    }
}

StartScreenComponent::~StartScreenComponent()
{
}

void StartScreenComponent::paint(juce::Graphics &g)
{
    // 전체 배경을 F5F5F0 색상으로 채움
    g.fillAll(juce::Colour(0xFFF5F5F0));

    // 전체 화면을 좌우 절반으로 분할
    auto bounds = getLocalBounds();
    auto leftHalf = bounds.removeFromLeft(bounds.getWidth() / 2); // 왼쪽 절반 영역 분리

    // 왼쪽 패널: 베이지색 배경과 둥근 모서리 적용
    g.setColour(juce::Colour(0xFFD9C5B2));
    g.fillRoundedRectangle(leftHalf.toFloat(), 20.0f); // 20픽셀 반경의 둥근 모서리

    // 로고 이미지를 왼쪽 패널 중앙에 배치 (50픽셀 여백)
    if (logoImage.isValid())
    {
        g.drawImage(logoImage,
                    leftHalf.reduced(50).toFloat(), // 모든 방향에서 50픽셀 여백
                    juce::RectanglePlacement::centred);
    }

    // 오른쪽 패널과 로그인 버튼 그리기
    drawRightPanel(g, bounds);
    drawLoginButton(g);
}

void StartScreenComponent::drawRightPanel(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    // 전체 패널에 50픽셀 여백 적용
    auto area = bounds.reduced(50);
    auto panelWidth = area.getWidth(); // 오른쪽 패널의 너비

    // 헤더 섹션 (150픽셀 높이)
    auto headerArea = area.removeFromTop(150);

    // "MAPLE" 로고 (상단 100픽셀)
    g.setFont(titleFont);
    g.setColour(juce::Colours::black);
    g.drawText(juce::String("MAPLE"), headerArea.removeFromTop(100), juce::Justification::topRight, true);

    // "Music Analysis" 텍스트 (남은 50픽셀)
    g.setFont(descriptionFont);
    g.setColour(juce::Colour(0xFF666666));
    g.drawText(juce::String("Music Analysis"), headerArea, juce::Justification::topRight, true);

    // 로그인 섹션과 헤더 사이 50픽셀 여백
    area.removeFromTop(50);
    auto loginArea = area;
    auto inputFieldWidth = panelWidth / 2; // 입력 필드 너비를 패널 너비의 절반으로 설정

    // "Log in" 제목 (60픽셀 높이)
    g.setFont(titleFont.withHeight(48.0f));
    g.setColour(juce::Colours::black);
    g.drawText(juce::String("Log in"), loginArea.removeFromTop(60).reduced((panelWidth - inputFieldWidth) / 2, 5), juce::Justification::left, true);

    // "Customization Settings" 텍스트 (30픽셀 여백 + 30픽셀 높이)
    loginArea.removeFromTop(30); // 여백
    g.setFont(descriptionFont);
    g.drawText(juce::String("ID"), loginArea.removeFromTop(30).reduced((panelWidth - inputFieldWidth) / 2, 0), juce::Justification::left, true);

    // 기타 상세 입력 필드 (20픽셀 여백 + 50픽셀 높이)
    loginArea.removeFromTop(10);
    // 입력 필드 너비 조정: reduced 함수의 첫 번째 인수를 사용하여 좌우 여백 설정
    auto inputField = loginArea.removeFromTop(50).reduced((panelWidth - inputFieldWidth) / 2, 0);
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(inputField.toFloat(), 10.0f);
    g.setColour(juce::Colour(0xFFCCCCCC));
    g.drawRoundedRectangle(inputField.toFloat(), 10.0f, 1.0f);
    g.setColour(juce::Colour(0xFFAAAAAA));
    g.setFont(descriptionFont);
    g.drawText(juce::String("Enter your guitar details"), inputField.reduced(15, 0), juce::Justification::left, true);

    // 비밀번호 레이블 (30픽셀 여백 + 30픽셀 높이)
    loginArea.removeFromTop(30); // 여백
    g.setColour(juce::Colours::black);
    g.drawText(juce::String("Password"), loginArea.removeFromTop(30).reduced((panelWidth - inputFieldWidth) / 2, 5), juce::Justification::left, true);

    // 비밀번호 입력 필드 (10픽셀 여백 + 50픽셀 높이)
    loginArea.removeFromTop(10);
    // 입력 필드 너비 조정: reduced 함수의 첫 번째 인수를 사용하여 좌우 여백 설정
    inputField = loginArea.removeFromTop(50).reduced((panelWidth - inputFieldWidth) / 2, 0);
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(inputField.toFloat(), 10.0f);
    g.setColour(juce::Colour(0xFFCCCCCC));
    g.drawRoundedRectangle(inputField.toFloat(), 10.0f, 1.0f);
    g.setColour(juce::Colour(0xFFAAAAAA));
    g.drawText(juce::String("•••••••••••••"), inputField.reduced(15, 0), juce::Justification::left, true);

    // "Forgot your password?" 링크 (20픽셀 여백 + 30픽셀 높이)
    loginArea.removeFromTop(20); // 여백
    g.setColour(juce::Colours::black);
    g.setFont(descriptionFont);
    g.drawText(juce::String("Forgot your password?"), loginArea.removeFromTop(30).reduced((panelWidth - inputFieldWidth) / 2, 5), juce::Justification::left, true);
}

void StartScreenComponent::drawLoginButton(juce::Graphics &g)
{
    auto buttonBounds = loginButtonBounds.toFloat();

    // Button background
    g.setColour(juce::Colour(0xFFD9C5B2));
    g.fillRoundedRectangle(buttonBounds, 10.0f);

    // Button text
    g.setColour(juce::Colours::white);
    g.setFont(descriptionFont.withHeight(16.0f));
    g.drawText("Log in", buttonBounds, juce::Justification::centred, false);
}

void StartScreenComponent::resized()
{
    // 전체 화면을 좌우로 분할
    auto bounds = getLocalBounds();
    auto rightHalf = bounds.removeFromRight(bounds.getWidth() / 2);

    // 로그인 버튼 위치 계산
    auto buttonArea = rightHalf.reduced(50);                   // 50픽셀 여백
    auto contentHeight = 100;              // 콘텐츠의 총 높이
    int startY = (buttonArea.getHeight() - contentHeight) / 2; // 수직 중앙 정렬
    buttonArea.removeFromTop(startY + contentHeight + 50);     // 콘텐츠 아래 위치

    // 로그인 버튼 크기 설정 (300x50 픽셀)
    loginButtonBounds = buttonArea.removeFromTop(50).withSizeKeepingCentre(300, 50);
}

void StartScreenComponent::mouseMove(const juce::MouseEvent &event)
{
    bool wasOver = isLoginButtonMouseOver;
    isLoginButtonMouseOver = isMouseOverButton(event.position);

    if (wasOver != isLoginButtonMouseOver)
    {
        repaint();
    }
}

void StartScreenComponent::mouseEnter(const juce::MouseEvent &event)
{
    bool wasOver = isLoginButtonMouseOver;
    isLoginButtonMouseOver = isMouseOverButton(event.position);

    if (wasOver != isLoginButtonMouseOver)
    {
        repaint();
    }
}

void StartScreenComponent::mouseExit(const juce::MouseEvent &event)
{
    isLoginButtonMouseOver = false;
    repaint();
}

void StartScreenComponent::mouseDown(const juce::MouseEvent &event)
{
    if (isMouseOverButton(event.position))
    {
        isLoginButtonDown = true;
        repaint();
    }
}

void StartScreenComponent::mouseUp(const juce::MouseEvent &event)
{
    if (isLoginButtonDown && isMouseOverButton(event.position))
    {
        // Navigate to Home screen when login button is clicked
        screenChangeCallback(Screen::Home);
    }
    isLoginButtonDown = false;
    repaint();
}

bool StartScreenComponent::isMouseOverButton(const juce::Point<float> &position) const
{
    return loginButtonBounds.toFloat().contains(position);
}