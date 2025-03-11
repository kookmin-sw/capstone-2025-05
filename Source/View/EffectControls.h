#pragma once
#include <JuceHeader.h>

class SquareEffectButton : public juce::Component {
public:
    SquareEffectButton(const juce::String& name);
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool isOn() const { return onState; }
    void setOn(bool state) { onState = state; repaint(); }

private:
    juce::String buttonName;
    bool onState = false;
};

class EffectControls : public juce::Component {
public:
    EffectControls();
    ~EffectControls() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    SquareEffectButton distortionButton{"Distortion"};
    SquareEffectButton delayButton{"Delay"};
    SquareEffectButton reverbButton{"Reverb"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectControls)
};