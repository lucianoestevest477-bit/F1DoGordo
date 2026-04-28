#pragma once

#include "AudioModule.h"

class FETCompressorModule final : public AudioModule
{
public:
    void process(juce::AudioBuffer<float>& buffer) override;
    float getGainReductionDb() const noexcept { return gainReductionDb; }

private:
    float gainReductionDb = 0.0f;
};
