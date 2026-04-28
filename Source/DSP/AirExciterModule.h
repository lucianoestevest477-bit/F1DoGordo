#pragma once

#include "AudioModule.h"

class AirExciterModule final : public AudioModule
{
public:
    void process(juce::AudioBuffer<float>& buffer) override;
};
