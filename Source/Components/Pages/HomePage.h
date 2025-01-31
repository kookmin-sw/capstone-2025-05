#pragma once
#include "Page.h"

class HomePage : public Page
{
public:
    HomePage();
    ~HomePage() override;

    juce::String getPageName() const override { return "Home"; }
    void resized() override;

private:
    // 페이지 컴포넌트들...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomePage)
};