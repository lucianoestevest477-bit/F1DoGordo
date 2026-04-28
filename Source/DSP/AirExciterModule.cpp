#include "AirExciterModule.h"

#include <cmath>

namespace
{
    constexpr auto filterQ = 0.70710678f;
}

void AirExciterModule::setParameters(ParameterPointers newParameters) noexcept
{
    parameters = newParameters;
}

void AirExciterModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    AudioModule::prepare(spec);

    prepared = spec.sampleRate > 0.0 && spec.numChannels > 0 && spec.maximumBlockSize > 0;

    if (! prepared)
        return;

    highPassFilter.prepare(spec);
    toneShelfFilter.prepare(spec);

    dryBuffer.setSize(static_cast<int>(spec.numChannels),
                      static_cast<int>(spec.maximumBlockSize),
                      false,
                      false,
                      true);
    highBuffer.setSize(static_cast<int>(spec.numChannels),
                       static_cast<int>(spec.maximumBlockSize),
                       false,
                       false,
                       true);

    amount.reset(spec.sampleRate, 0.02);
    drive.reset(spec.sampleRate, 0.02);
    outputGain.reset(spec.sampleRate, 0.02);
    mix.reset(spec.sampleRate, 0.02);

    amount.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airAmount, 0.0f)));
    drive.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDrive, 0.0f)));
    outputGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(getParameterValue(parameters.airOutputDb, 0.0f)));
    mix.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airMix, 1.0f)));

    cached = {};
    updateFilterCoefficients(true);
    reset();
}

void AirExciterModule::reset()
{
    highPassFilter.reset();
    toneShelfFilter.reset();
}

void AirExciterModule::process(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    if (! prepared || sampleRate <= 0.0)
        return;

    const auto enabled = getParameterValue(parameters.airEnabled, 1.0f) > 0.5f;
    const auto targetMix = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airMix, 1.0f));

    if (! enabled || targetMix <= 0.0001f)
        return;

    const auto bufferChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (bufferChannels <= 0 || numSamples <= 0 || bufferChannels > dryBuffer.getNumChannels() || numSamples > dryBuffer.getNumSamples())
        return;

    const auto targetAmount = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airAmount, 0.0f));
    const auto targetDrive = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDrive, 0.0f));
    const auto targetOutputGain = juce::Decibels::decibelsToGain(juce::jlimit(-12.0f, 12.0f, getParameterValue(parameters.airOutputDb, 0.0f)));

    if (targetAmount <= 0.0001f && targetDrive <= 0.0001f && std::abs(targetOutputGain - 1.0f) <= 0.0001f)
        return;

    amount.setTargetValue(targetAmount);
    drive.setTargetValue(targetDrive);
    outputGain.setTargetValue(targetOutputGain);
    mix.setTargetValue(targetMix);
    updateFilterCoefficients(false);

    for (auto channel = 0; channel < bufferChannels; ++channel)
    {
        dryBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
        highBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    }

    auto highBlock = juce::dsp::AudioBlock<float>(highBuffer).getSubBlock(0, static_cast<size_t>(numSamples));
    auto context = juce::dsp::ProcessContextReplacing<float>(highBlock);
    highPassFilter.process(context);
    toneShelfFilter.process(context);

    for (auto sample = 0; sample < numSamples; ++sample)
    {
        const auto currentAmount = amount.getNextValue();
        const auto currentDrive = drive.getNextValue();
        const auto currentOutputGain = outputGain.getNextValue();
        const auto currentMix = mix.getNextValue();
        const auto harmonicDrive = 1.0f + currentDrive * 8.0f;
        const auto harmonicBlend = currentAmount * (1.25f + currentDrive * 1.75f);
        const auto sheenBlend = currentAmount * (0.35f + currentDrive * 0.25f);
        const auto normaliser = 1.0f / std::tanh(harmonicDrive);

        for (auto channel = 0; channel < bufferChannels; ++channel)
        {
            auto* samples = buffer.getWritePointer(channel);
            const auto dry = dryBuffer.getReadPointer(channel)[sample];
            const auto high = highBuffer.getReadPointer(channel)[sample];
            const auto safeDry = std::isfinite(dry) ? dry : 0.0f;
            const auto safeHigh = std::isfinite(high) ? high : 0.0f;

            const auto saturatedHigh = std::tanh(safeHigh * harmonicDrive) * normaliser;
            const auto harmonics = saturatedHigh - safeHigh;
            auto wet = safeDry + safeHigh * sheenBlend + harmonics * harmonicBlend;
            wet *= currentOutputGain;

            if (! std::isfinite(wet))
                wet = 0.0f;

            if (std::abs(wet) > 3.0f)
                wet = std::tanh(wet / 3.0f) * 3.0f;

            const auto outputSample = safeDry * (1.0f - currentMix) + wet * currentMix;
            samples[sample] = std::isfinite(outputSample) ? outputSample : 0.0f;
        }
    }

    sanitizeBuffer(buffer);
}

void AirExciterModule::updateFilterCoefficients(bool force)
{
    const auto frequency = sanitizeFrequency(getParameterValue(parameters.airFrequencyHz, 8000.0f));
    const auto tone = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airTone, 0.5f));
    const auto frequencyChanged = frequency != cached.frequencyHz;

    if (force || frequencyChanged)
    {
        *highPassFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeHighPass(sampleRate, frequency, filterQ);
        cached.frequencyHz = frequency;
    }

    if (force || frequencyChanged || tone != cached.tone)
    {
        const auto shelfFrequency = juce::jlimit(3000.0f,
                                                 static_cast<float>(sampleRate * 0.45),
                                                 frequency * 1.15f);
        const auto shelfGainDb = -3.0f + tone * 9.0f;
        *toneShelfFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeHighShelf(sampleRate,
                                                                                         shelfFrequency,
                                                                                         filterQ,
                                                                                         juce::Decibels::decibelsToGain(shelfGainDb));
        cached.tone = tone;
    }
}

float AirExciterModule::getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept
{
    if (parameter == nullptr)
        return fallback;

    const auto value = parameter->load();
    return std::isfinite(value) ? value : fallback;
}

float AirExciterModule::sanitizeFrequency(float value) const noexcept
{
    const auto nyquistLimitedMax = static_cast<float>(sampleRate * 0.45);
    const auto safeMax = juce::jmax(2001.0f, juce::jmin(16000.0f, nyquistLimitedMax));
    const auto safeValue = std::isfinite(value) ? value : 8000.0f;

    return juce::jlimit(2000.0f, safeMax, safeValue);
}

void AirExciterModule::sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept
{
    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            if (! std::isfinite(samples[sample]))
                samples[sample] = 0.0f;
    }
}
