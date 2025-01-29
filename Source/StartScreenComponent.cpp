#include "StartScreenComponent.h"

StartScreenComponent::StartScreenComponent(std::function<void(Screen)> callback)
    : screenChangeCallback(std::move(callback))
{
    // Set up fonts
    auto options = juce::Font::getDefaultSansSerifFontName();
    titleFont = juce::Font(options, 72.0f, juce::Font::plain);
    descriptionFont = juce::Font(options, 20.0f, juce::Font::plain);

    // ID 필드 설정
    addAndMakeVisible(idField);
    idField.setLookAndFeel(&roundedLookAndFeel);
    idField.setTextToShowWhenEmpty("Enter your username", juce::Colours::grey);
    idField.setFont(descriptionFont);
    idField.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    idField.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFFCCCCCC));
    idField.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xFFCCCCCC));
    idField.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    idField.setColour(juce::CaretComponent::caretColourId, juce::Colours::black);
    idField.setMultiLine(false);
    idField.setReturnKeyStartsNewLine(false);
    idField.setIndents(10, 5);
    idField.setJustification(juce::Justification::centredLeft);

    // 비밀번호 필드 설정
    addAndMakeVisible(passwordField);
    passwordField.setLookAndFeel(&roundedLookAndFeel);
    passwordField.setTextToShowWhenEmpty("Password", juce::Colours::grey);
    passwordField.setFont(descriptionFont);
    passwordField.setPasswordCharacter('•');
    passwordField.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    passwordField.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFFCCCCCC));
    passwordField.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xFFCCCCCC));
    passwordField.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    passwordField.setColour(juce::CaretComponent::caretColourId, juce::Colours::black);
    passwordField.setMultiLine(false);
    passwordField.setReturnKeyStartsNewLine(false);
    passwordField.setIndents(10, 5);
    passwordField.setJustification(juce::Justification::centredLeft);

    // Enable mouse events
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
    idField.setLookAndFeel(nullptr);
    passwordField.setLookAndFeel(nullptr);
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
    auto panelWidth = area.getWidth();
    auto inputFieldWidth = panelWidth / 2;

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

    // "Log in" 제목 (60픽셀 높이)
    g.setFont(titleFont.withHeight(48.0f));
    g.setColour(juce::Colours::black);
    g.drawText(juce::String("Log in"), loginArea.removeFromTop(60).reduced((panelWidth - inputFieldWidth) / 2, 5), juce::Justification::left, true);

    // "Username" 텍스트
    loginArea.removeFromTop(30);
    g.setFont(descriptionFont);
    g.drawText(juce::String("Username"), loginArea.removeFromTop(30).reduced((panelWidth - inputFieldWidth) / 2, 0), juce::Justification::left, true);

    // ID 필드 공간 (여기서는 아무것도 그리지 않음)
    loginArea.removeFromTop(10);
    loginArea.removeFromTop(50);

    // "Password" 텍스트
    loginArea.removeFromTop(30);
    g.setColour(juce::Colours::black);
    g.drawText(juce::String("Password"), loginArea.removeFromTop(30).reduced((panelWidth - inputFieldWidth) / 2, 5), juce::Justification::left, true);

    // 비밀번호 필드 공간 (여기서는 아무것도 그리지 않음)
    loginArea.removeFromTop(10);
    loginArea.removeFromTop(50);

    // "Forgot your password?" 링크
    loginArea.removeFromTop(20);
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

    // 오른쪽 패널 영역 계산
    auto area = rightHalf.reduced(50);
    auto panelWidth = area.getWidth();
    auto inputFieldWidth = panelWidth / 2;

    // 헤더와 "Log in" 제목 공간 확보
    auto loginArea = area;
    loginArea.removeFromTop(150 + 50 + 60 + 30); // header + spacing + "Log in" + spacing

    // ID 필드 위치 설정
    loginArea.removeFromTop(30); // "Username" label
    loginArea.removeFromTop(10); // spacing
    auto idBounds = loginArea.removeFromTop(50).reduced((panelWidth - inputFieldWidth) / 2, 0);
    idField.setBounds(idBounds);

    // 비밀번호 필드 위치 설정
    loginArea.removeFromTop(30); // spacing
    loginArea.removeFromTop(30); // "Password" label
    loginArea.removeFromTop(10); // spacing
    auto passwordBounds = loginArea.removeFromTop(50).reduced((panelWidth - inputFieldWidth) / 2, 0);
    passwordField.setBounds(passwordBounds);

    // 로그인 버튼 위치 계산
    auto buttonArea = rightHalf.reduced(50);
    auto contentHeight = 100;
    int startY = (buttonArea.getHeight() - contentHeight) / 2;
    buttonArea.removeFromTop(startY + contentHeight + 50);

    // 로그인 버튼 크기 설정 (300x50 픽셀)
    loginButtonBounds = buttonArea.removeFromTop(50).withSizeKeepingCentre(300, 50);
}

void StartScreenComponent::mouseMove(const juce::MouseEvent &event)
{
    bool wasOver = isLoginButtonMouseOver;
    isLoginButtonMouseOver = isMouseOverButton(event.position);

    // 로그인 버튼 위에 있을 때만 커서 변경
    if (isLoginButtonMouseOver)
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    else
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }

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