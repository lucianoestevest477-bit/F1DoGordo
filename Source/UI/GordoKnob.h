#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class GordoKnob final : public juce::Slider
{
public:
    explicit GordoKnob(const juce::String& labelText);

    void paint(juce::Graphics& g) override;
    void setLabelText(const juce::String& newLabelText);

private:
    juce::String label;
};
