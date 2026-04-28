#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class GordoMeter final : public juce::Component
{
public:
    explicit GordoMeter(const juce::String& labelText);

    void setLevel(float newLevel);
    void setReductionMode(bool shouldUseReductionMode) noexcept { reductionMode = shouldUseReductionMode; }
    void paint(juce::Graphics& g) override;

private:
    juce::String label;
    float level = 0.0f;
    bool reductionMode = false;
};
