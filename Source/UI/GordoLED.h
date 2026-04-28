#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class GordoLED final : public juce::Component
{
public:
    GordoLED();

    void setLedColour(juce::Colour newColour);
    void setOn(bool shouldBeOn);
    void paint(juce::Graphics& g) override;

private:
    juce::Colour colour;
    bool on = false;
};
