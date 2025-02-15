#pragma once
#include <JuceHeader.h>
#include "../Styles/MapleColours.h"
#include "../../Components/MainComponent.h"

class ProjectCreateDialog : public juce::DialogWindow
{
public:
    ProjectCreateDialog(const juce::String& name)
        : DialogWindow(name, MapleColours::currentTheme.panel, true)
    {
        setUsingNativeTitleBar(true);
        setResizable(false, false);
        
        setContentOwned(new ProjectCreateComponent(), true);
        centreWithSize(800, 400);
    }

    void closeButtonPressed() override
    {
        setVisible(false);
    }

    static void show(MainComponent& mainComponent)
    {
        DialogWindow::LaunchOptions options;
        options.dialogTitle = juce::CharPointer_UTF8(u8"새 프로젝트 만들기");
        auto* content = new ProjectCreateComponent();
        
        content->onModeSelected = [&mainComponent]() {
            mainComponent.setMode(MainComponent::Mode::Project);
        };
        
        options.content.setOwned(content);
        options.componentToCentreAround = nullptr;
        options.dialogBackgroundColour = MapleColours::currentTheme.panel;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = false;

        // 다이얼로그 크기 지정
        content->setSize(800, 400);

        options.launchAsync();
    }

private:
    class ProjectCreateComponent : public juce::Component
    {
    public:
        ProjectCreateComponent()
        {
            setupCards();
            
            addAndMakeVisible(*readyToPlayCard);
            addAndMakeVisible(*practiceScaleCard);
            addAndMakeVisible(*practiceChromaticsCard);
        }

        void resized() override
        {
            auto bounds = getLocalBounds().reduced(20);
            const int cardWidth = (bounds.getWidth() - 40) / 3;
            const int cardHeight = bounds.getHeight();

            readyToPlayCard->setBounds(bounds.removeFromLeft(cardWidth));
            bounds.removeFromLeft(20);
            practiceScaleCard->setBounds(bounds.removeFromLeft(cardWidth));
            bounds.removeFromLeft(20);
            practiceChromaticsCard->setBounds(bounds);
        }

        void paint(juce::Graphics& g) override
        {
            g.fillAll(MapleColours::currentTheme.panel);
        }

        std::function<void()> onModeSelected;

    private:
        class ProjectCard : public juce::Component
        {
        public:
            ProjectCard(const juce::String& title, const juce::String& description)
                : titleText(title), descriptionText(description)
            {
                setMouseCursor(juce::MouseCursor::PointingHandCursor);
            }

            std::function<void()> onClick;

            void mouseUp(const juce::MouseEvent&) override
            {
                if (onClick)
                    onClick();
            }

            void paint(juce::Graphics& g) override
            {
                auto bounds = getLocalBounds().toFloat().reduced(5);
                
                g.setColour(juce::Colour(40, 40, 40));
                g.fillRoundedRectangle(bounds, 10.0f);

                g.setColour(juce::Colour(76, 217, 100));
                g.setFont(20.0f);
                g.drawText(titleText, bounds.removeFromTop(40),
                          juce::Justification::centred);

                g.setColour(juce::Colours::white);
                g.setFont(16.0f);
                g.drawFittedText(descriptionText,
                               bounds.reduced(10).toNearestInt(),
                               juce::Justification::centred,
                               3);
            }

            void mouseEnter(const juce::MouseEvent&) override
            {
                isMouseOver = true;
                repaint();
            }

            void mouseExit(const juce::MouseEvent&) override
            {
                isMouseOver = false;
                repaint();
            }

        private:
            juce::String titleText;
            juce::String descriptionText;
            bool isMouseOver = false;
        };

        void setupCards()
        {
            readyToPlayCard = std::make_unique<ProjectCard>(
                juce::CharPointer_UTF8(u8"시작하기"),
                juce::CharPointer_UTF8(u8"커버하고 싶은 곡을 선택하여 연습을 시작하세요.")
            );
            
            readyToPlayCard->onClick = [this]() {
                if (onModeSelected)
                    onModeSelected();
                
                if (auto* dlg = findParentComponentOfClass<DialogWindow>())
                    dlg->exitModalState(1);
            };

            practiceScaleCard = std::make_unique<ProjectCard>(
                juce::CharPointer_UTF8(u8"스케일 연습 모드"),
                juce::CharPointer_UTF8(u8"스케일 연습과 어휘력 향상을 통해 언어 능력을 개선하세요.")
            );

            practiceChromaticsCard = std::make_unique<ProjectCard>(
                juce::CharPointer_UTF8(u8"크로매틱 연습"),
                juce::CharPointer_UTF8(u8"크로매틱 연습으로 언어 실력을 미세 조정하세요.")
            );
        }

        std::unique_ptr<ProjectCard> readyToPlayCard;
        std::unique_ptr<ProjectCard> practiceScaleCard;
        std::unique_ptr<ProjectCard> practiceChromaticsCard;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectCreateComponent)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectCreateDialog)
}; 