#pragma once

#include <juce_dsp/juce_dsp.h>

class AudioModule
{
public:
    virtual ~AudioModule() = default;

    virtual void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        maxBlockSize = spec.maximumBlockSize;
        numChannels = spec.numChannels;
    }

    virtual void reset() {}
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;

protected:
    double sampleRate = 44100.0;
    juce::uint32 maxBlockSize = 0;
    juce::uint32 numChannels = 0;
};
