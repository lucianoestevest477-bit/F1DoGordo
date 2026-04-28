#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class GordoButton final : public juce::Button
{
public:
    explicit GordoButton(const juce::String& text);

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};
