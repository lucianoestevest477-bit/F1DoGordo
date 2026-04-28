#include "ChannelModule.h"

#include <cmath>
#include <limits>

namespace
{
    constexpr auto filterQ = 0.70710678f;
    constexpr auto midQ = 0.85f;
}

void ChannelModule::setParameters(ParameterPointers newParameters) noexcept
{
    parameters = newParameters;
}

void ChannelModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    AudioModule::prepare(spec);

    prepared = spec.sampleRate > 0.0 && spec.numChannels > 0 && spec.maximumBlockSize > 0;

    if (! prepared)
        return;

    highPassFilter.prepare(spec);
    lowPassFilter.prepare(spec);
    lowShelfFilter.prepare(spec);
    lowMidFilter.prepare(spec);
    highMidFilter.prepare(spec);
    highShelfFilter.prepare(spec);

    dryBuffer.setSize(static_cast<int>(spec.numChannels),
                      static_cast<int>(spec.maximumBlockSize),
                      false,
                      false,
                      true);

    cached = {};
    updateFilterCoefficients(true);
    reset();
}

void ChannelModule::reset()
{
    highPassFilter.reset();
    lowPassFilter.reset();
    lowShelfFilter.reset();
    lowMidFilter.reset();
    highMidFilter.reset();
    highShelfFilter.reset();
}

void ChannelModule::process(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    if (! prepared || sampleRate <= 0.0)
        return;

    const auto enabled = getParameterValue(parameters.channelEnabled, 1.0f) > 0.5f;
    const auto mix = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.channelMix, 1.0f));

    if (! enabled || mix <= 0.0001f)
        return;

    const auto bufferChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (bufferChannels > dryBuffer.getNumChannels() || numSamples > dryBuffer.getNumSamples())
        return;

    if (mix < 0.9999f)
        for (auto channel = 0; channel < bufferChannels; ++channel)
            dryBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);

    updateFilterCoefficients(false);

    const auto inputTrimDb = juce::jlimit(-18.0f, 18.0f, getParameterValue(parameters.channelInputTrimDb, 0.0f));
    buffer.applyGain(juce::Decibels::decibelsToGain(inputTrimDb));

    if (getParameterValue(parameters.phaseInvert, 0.0f) > 0.5f)
        buffer.applyGain(-1.0f);

    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    highPassFilter.process(context);
    lowPassFilter.process(context);
    lowShelfFilter.process(context);
    lowMidFilter.process(context);
    highMidFilter.process(context);
    highShelfFilter.process(context);

    applyDrive(buffer, getParameterValue(parameters.drive, 0.0f));

    if (mix < 0.9999f)
    {
        const auto dryGain = 1.0f - mix;

        for (auto channel = 0; channel < bufferChannels; ++channel)
        {
            auto* wet = buffer.getWritePointer(channel);
            const auto* dry = dryBuffer.getReadPointer(channel);

            for (auto sample = 0; sample < numSamples; ++sample)
                wet[sample] = dry[sample] * dryGain + wet[sample] * mix;
        }
    }

    sanitizeBuffer(buffer);
}

void ChannelModule::updateFilterCoefficients(bool force)
{
    const auto highPass = sanitizeFrequency(getParameterValue(parameters.highPassHz, 20.0f), 20.0f, 1000.0f);
    const auto lowPass = sanitizeFrequency(getParameterValue(parameters.lowPassHz, 20000.0f), 2000.0f, 20000.0f);
    const auto lowGain = juce::jlimit(-12.0f, 12.0f, getParameterValue(parameters.lowGainDb, 0.0f));
    const auto lowFreq = sanitizeFrequency(getParameterValue(parameters.lowFreqHz, 100.0f), 40.0f, 400.0f);
    const auto lowMidGain = juce::jlimit(-12.0f, 12.0f, getParameterValue(parameters.lowMidGainDb, 0.0f));
    const auto lowMidFreq = sanitizeFrequency(getParameterValue(parameters.lowMidFreqHz, 450.0f), 150.0f, 2000.0f);
    const auto highMidGain = juce::jlimit(-12.0f, 12.0f, getParameterValue(parameters.highMidGainDb, 0.0f));
    const auto highMidFreq = sanitizeFrequency(getParameterValue(parameters.highMidFreqHz, 3000.0f), 1000.0f, 8000.0f);
    const auto highGain = juce::jlimit(-12.0f, 12.0f, getParameterValue(parameters.highGainDb, 0.0f));
    const auto highFreq = sanitizeFrequency(getParameterValue(parameters.highFreqHz, 8000.0f), 3000.0f, 16000.0f);

    if (force || highPass != cached.highPassHz)
    {
        *highPassFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeHighPass(sampleRate, highPass, filterQ);
        cached.highPassHz = highPass;
    }

    if (force || lowPass != cached.lowPassHz)
    {
        *lowPassFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeLowPass(sampleRate, lowPass, filterQ);
        cached.lowPassHz = lowPass;
    }

    if (force || lowGain != cached.lowGainDb || lowFreq != cached.lowFreqHz)
    {
        *lowShelfFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeLowShelf(sampleRate,
                                                                                       lowFreq,
                                                                                       filterQ,
                                                                                       juce::Decibels::decibelsToGain(lowGain));
        cached.lowGainDb = lowGain;
        cached.lowFreqHz = lowFreq;
    }

    if (force || lowMidGain != cached.lowMidGainDb || lowMidFreq != cached.lowMidFreqHz)
    {
        *lowMidFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter(sampleRate,
                                                                                       lowMidFreq,
                                                                                       midQ,
                                                                                       juce::Decibels::decibelsToGain(lowMidGain));
        cached.lowMidGainDb = lowMidGain;
        cached.lowMidFreqHz = lowMidFreq;
    }

    if (force || highMidGain != cached.highMidGainDb || highMidFreq != cached.highMidFreqHz)
    {
        *highMidFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter(sampleRate,
                                                                                        highMidFreq,
                                                                                        midQ,
                                                                                        juce::Decibels::decibelsToGain(highMidGain));
        cached.highMidGainDb = highMidGain;
        cached.highMidFreqHz = highMidFreq;
    }

    if (force || highGain != cached.highGainDb || highFreq != cached.highFreqHz)
    {
        *highShelfFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeHighShelf(sampleRate,
                                                                                        highFreq,
                                                                                        filterQ,
                                                                                        juce::Decibels::decibelsToGain(highGain));
        cached.highGainDb = highGain;
        cached.highFreqHz = highFreq;
    }
}

float ChannelModule::getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept
{
    if (parameter == nullptr)
        return fallback;

    const auto value = parameter->load();
    return std::isfinite(value) ? value : fallback;
}

float ChannelModule::sanitizeFrequency(float value, float minHz, float maxHz) const noexcept
{
    const auto nyquistLimitedMax = static_cast<float>(sampleRate * 0.49);
    const auto safeMax = juce::jmax(minHz + 1.0f, juce::jmin(maxHz, nyquistLimitedMax));
    const auto safeValue = std::isfinite(value) ? value : minHz;

    return juce::jlimit(minHz, safeMax, safeValue);
}

void ChannelModule::applyDrive(juce::AudioBuffer<float>& buffer, float amount) const noexcept
{
    amount = juce::jlimit(0.0f, 1.0f, std::isfinite(amount) ? amount : 0.0f);

    if (amount <= 0.0001f)
        return;

    const auto preGain = 1.0f + amount * 5.0f;
    const auto wetAmount = amount * 0.25f;
    const auto dryAmount = 1.0f - wetAmount;
    const auto safetyTrim = 1.0f - amount * 0.05f;
    const auto normaliser = 1.0f / std::tanh(preGain);

    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const auto input = samples[sample];
            const auto safeInput = std::isfinite(input) ? input : 0.0f;
            const auto saturated = std::tanh(safeInput * preGain) * normaliser;
            samples[sample] = (safeInput * dryAmount + saturated * wetAmount) * safetyTrim;
        }
    }
}

void ChannelModule::sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept
{
    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            if (! std::isfinite(samples[sample]))
                samples[sample] = 0.0f;
    }
}
