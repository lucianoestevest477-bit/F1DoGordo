#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "GordoButton.h"

class F1TopBar final : public juce::Component
{
public:
    F1TopBar();

    void setCpuText(const juce::String& text);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label presetLabel;
    juce::Label qualityLabel;
    juce::Label cpuLabel;
    GordoButton abButton { "A/B" };
    GordoButton saveButton { "SAVE" };
};
