#include <JuceHeader.h>
#include "../Components/MainComponent.h"
#include "../Utils/Constants.h"
#include "../Windows/LoginWindow.h"
#include "../Config/AppConfig.h"
#include "../Services/SpotifyService.h"

class MapleApplication : public juce::JUCEApplication
{
public:
    MapleApplication() {}
    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        AppConfig::init();  // 환경 변수 초기화

        juce::Desktop::getInstance().setDefaultLookAndFeel(&lookAndFeel);  // 기본 스타일 설정
        
        // 로그인 윈도우 생성
        loginWindow = new LoginWindow(getApplicationName(), 
            // 로그인 성공 콜백
            [this]() {
                createMainWindow();
            },
            // 윈도우 닫힘 콜백
            [this]() {
                if (mainWindow == nullptr) {
                    // 메인 윈도우가 없는 상태에서 로그인 창이 닫히면 앱 종료
                    systemRequestedQuit();
                }
            });
    }

    void shutdown() override
    {
        // 리소스 정리
        SpotifyService::cleanup();
        
        // 윈도우 정리
        mainWindow = nullptr;
        loginWindow = nullptr;
    }

    void createMainWindow()
    {
        // 메인 윈도우 생성 전에 로그인 윈도우가 완전히 닫혔는지 확인
        if (loginWindow != nullptr)
        {
            loginWindow = nullptr;
        }

        // 메인 윈도우 생성
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                           juce::Desktop::getInstance().getDefaultLookAndFeel()
                               .findColour(juce::ResizableWindow::backgroundColourId),
                           DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            setResizable(true, true);
            
            // 윈도우를 보이기 전에 크기와 위치 설정
            centreWithSize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
            
            // 모든 설정이 완료된 후에 윈도우를 보이게 함
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    juce::LookAndFeel_V4 lookAndFeel;  // 기본 스타일 객체 추가
    std::unique_ptr<MainWindow> mainWindow;
    LoginWindow* loginWindow = nullptr;
};

START_JUCE_APPLICATION(MapleApplication)