#include "LoginWindow.h"

LoginWindow::LoginWindow(const juce::String& name, 
                        std::function<void()> onLoginSuccessCallback,
                        std::function<void()> onWindowCloseCallback)
    : DocumentWindow(name,
                    juce::Desktop::getInstance().getDefaultLookAndFeel()
                        .findColour(juce::ResizableWindow::backgroundColourId),
                    DocumentWindow::allButtons),
      onWindowClose(std::move(onWindowCloseCallback))
{
    // 윈도우가 보이기 전에 모든 초기화 완료
    setUsingNativeTitleBar(true);
    setResizable(false, false);
    setDropShadowEnabled(true);
    setAlwaysOnTop(true);

    loginComponent = new LoginComponent([this, onLoginSuccessCallback]() {
        onLoginSuccessCallback();
        closeButtonPressed();
    });

    setContentOwned(loginComponent, true);
    
    // 크기와 위치 설정
    centreWithSize(900, 600);
    
    // 모든 설정이 완료된 후에만 윈도우를 보이게 함
    setVisible(true);
}

void LoginWindow::setupWindow()
{
    // setupWindow 함수는 더 이상 필요하지 않음
}

void LoginWindow::closeButtonPressed()
{
    if (onWindowClose)
        onWindowClose();
    delete this;
}

// ... 기타 메서드들 