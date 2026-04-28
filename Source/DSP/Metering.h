#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class Metering
{
public:
    void reset() noexcept;
    void update(const juce::AudioBuffer<float>& buffer) noexcept;

    float getPeak() const noexcept { return peak; }
    float getRms() const noexcept { return rms; }

private:
    float peak = 0.0f;
    float rms = 0.0f;
};
