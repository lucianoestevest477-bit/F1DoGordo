#pragma once

#include "AudioModule.h"

class ReverbModule final : public AudioModule
{
public:
    void process(juce::AudioBuffer<float>& buffer) override;
};
