#pragma once

#include "AudioModule.h"

class DelayModule final : public AudioModule
{
public:
    void process(juce::AudioBuffer<float>& buffer) override;
};
