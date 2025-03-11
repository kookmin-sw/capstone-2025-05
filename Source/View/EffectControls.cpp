#include "EffectControls.h"

// 커스텀 버튼 구현
SquareEffectButton::SquareEffectButton(const juce::String& name) : buttonName(name) {
    setSize(80, 80); // 정사각형 크기 고정
}

void SquareEffectButton::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    g.setColour(onState ? juce::Colours::red : juce::Colours::darkgrey);
    g.fillRect(bounds);
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText(buttonName, bounds, juce::Justification::centred, true);
}

void SquareEffectButton::mouseDown(const juce::MouseEvent& e) {
    if (e.mods.isLeftButtonDown()) {
        onState = !onState; // 토글
        repaint();
    }
}

// EffectControls 구현
EffectControls::EffectControls() {
    addAndMakeVisible(distortionButton);
    addAndMakeVisible(delayButton);
    addAndMakeVisible(reverbButton);
}

EffectControls::~EffectControls() {}

void EffectControls::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void EffectControls::resized() {
    auto bounds = getLocalBounds();
    int buttonSize = 80;
    int totalWidth = (buttonSize * 3) + (10 * 2); // 버튼 3개 + 간격 2개
    auto centeredBounds = bounds.withSizeKeepingCentre(totalWidth, buttonSize);

    distortionButton.setBounds(centeredBounds.removeFromLeft(buttonSize));
    centeredBounds.removeFromLeft(10); // 간격
    delayButton.setBounds(centeredBounds.removeFromLeft(buttonSize));
    centeredBounds.removeFromLeft(10);
    reverbButton.setBounds(centeredBounds.removeFromLeft(buttonSize));
}