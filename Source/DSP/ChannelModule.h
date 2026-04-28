#pragma once

#include "AudioModule.h"

class ChannelModule final : public AudioModule
{
public:
    void process(juce::AudioBuffer<float>& buffer) override;
};
