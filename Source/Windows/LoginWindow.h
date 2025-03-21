#pragma once
#include <JuceHeader.h>
#include "../Components/Login/LoginComponent.h"

class LoginWindow : public juce::DocumentWindow
{
public:
    LoginWindow(const juce::String& name, 
               std::function<void()> onLoginSuccessCallback,
               std::function<void()> onWindowCloseCallback);
    
    void closeButtonPressed() override;

private:
    void setupWindow();
    LoginComponent* loginComponent;
    std::function<void()> onWindowClose;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoginWindow)
}; 