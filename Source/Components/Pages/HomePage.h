#pragma once
#include <JuceHeader.h>
#include "../../UI/Common/Button/MapleButton.h"

class HomePage : public juce::Component
{
public:
    HomePage();
    ~HomePage() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    juce::Label titleLabel;
    juce::Label welcomeLabel;
    MapleButton continueButton;
    MapleButton newCourseButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomePage)
};