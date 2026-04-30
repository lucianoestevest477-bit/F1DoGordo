#include "FETCompressorModule.h"

#include <cmath>

namespace
{
    constexpr auto maxSidechainChannels = 2;
    constexpr auto minDetectorDb = -100.0f;

    float coefficientForTime(double sampleRate, float timeSeconds) noexcept
    {
        if (sampleRate <= 0.0 || timeSeconds <= 0.0f)
            return 0.0f;

        return std::exp(-1.0f / static_cast<float>(sampleRate * static_cast<double>(timeSeconds)));
    }

    float normalise(float value, float minimum, float maximum) noexcept
    {
        return juce::jlimit(0.0f, 1.0f, (value - minimum) / (maximum - minimum));
    }

    float mapAttackToAudibleFetRange(float attackMs) noexcept
    {
        const auto normalised = normalise(juce::jlimit(0.02f, 2.0f, attackMs), 0.02f, 2.0f);
        return 0.05f + std::pow(normalised, 1.35f) * 34.95f;
    }

    float mapReleaseToAudibleFetRange(float releaseMs) noexcept
    {
        const auto normalised = normalise(juce::jlimit(50.0f, 1200.0f, releaseMs), 50.0f, 1200.0f);
        return 35.0f + std::pow(normalised, 1.12f) * 1565.0f;
    }

    float ratioAudibilityScale(float ratio) noexcept
    {
        const auto normalised = normalise(juce::jlimit(4.0f, 30.0f, ratio), 4.0f, 30.0f);
        return 0.88f + normalised * 0.72f;
    }
}

void FETCompressorModule::setParameters(ParameterPointers newParameters) noexcept
{
    parameters = newParameters;
}

void FETCompressorModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    AudioModule::prepare(spec);

    prepared = spec.sampleRate > 0.0 && spec.numChannels > 0 && spec.maximumBlockSize > 0;

    if (! prepared)
        return;

    dryBuffer.setSize(static_cast<int>(spec.numChannels),
                      static_cast<int>(spec.maximumBlockSize),
                      false,
                      false,
                      true);

    inputGain.reset(spec.sampleRate, 0.02);
    outputGain.reset(spec.sampleRate, 0.02);
    mix.reset(spec.sampleRate, 0.02);

    inputGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(getParameterValue(parameters.compInputDb, 0.0f)));
    outputGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(getParameterValue(parameters.compOutputDb, 0.0f)));
    mix.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.compMix, 1.0f)));

    reset();
}

void FETCompressorModule::reset()
{
    sidechainX1 = {};
    sidechainY1 = {};
    cachedSidechainHpHz = -1.0f;
    detectorEnvelope = 0.0f;
    displayedGainReductionDb = 0.0f;
    gainReductionDb.store(0.0f);
    inputPeak.store(0.0f);
    outputPeak.store(0.0f);

    if (sampleRate > 0.0)
        updateSidechainFilter(getParameterValue(parameters.compSidechainHpHz, 90.0f));
}

void FETCompressorModule::process(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    if (! prepared || sampleRate <= 0.0)
        return;

    const auto enabled = getParameterValue(parameters.compEnabled, 1.0f) > 0.5f;
    const auto targetMix = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.compMix, 1.0f));

    if (! enabled || targetMix <= 0.0001f)
    {
        storeBypassedMeterLevels(buffer);
        detectorEnvelope = 0.0f;
        displayedGainReductionDb = 0.0f;
        gainReductionDb.store(0.0f);
        return;
    }

    const auto bufferChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (bufferChannels <= 0 || numSamples <= 0 || bufferChannels > dryBuffer.getNumChannels() || numSamples > dryBuffer.getNumSamples())
        return;

    for (auto channel = 0; channel < bufferChannels; ++channel)
        dryBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);

    inputGain.setTargetValue(juce::Decibels::decibelsToGain(juce::jlimit(-24.0f, 24.0f, getParameterValue(parameters.compInputDb, 0.0f))));
    outputGain.setTargetValue(juce::Decibels::decibelsToGain(juce::jlimit(-24.0f, 24.0f, getParameterValue(parameters.compOutputDb, 0.0f))));
    mix.setTargetValue(targetMix);
    updateSidechainFilter(getParameterValue(parameters.compSidechainHpHz, 90.0f));

    auto attackMs = mapAttackToAudibleFetRange(getParameterValue(parameters.compAttack, 0.45f));
    auto releaseMs = mapReleaseToAudibleFetRange(getParameterValue(parameters.compRelease, 220.0f));
    auto thresholdDb = juce::jlimit(-60.0f, 0.0f, getParameterValue(parameters.compThresholdDb, -18.0f));
    auto gainReductionScale = 1.0f;
    auto saturationAmount = 0.0f;

    const auto revision = getRevision();

    if (revision == Revision::blue)
    {
        attackMs *= 0.65f;
        thresholdDb -= 1.5f;
        gainReductionScale = 1.06f;
    }
    else if (revision == Revision::black)
    {
        attackMs *= 1.2f;
        releaseMs *= 1.25f;
        thresholdDb += 1.0f;
        gainReductionScale = 0.92f;
    }
    else if (revision == Revision::hot)
    {
        thresholdDb -= 2.0f;
        gainReductionScale = 1.12f;
        saturationAmount = 0.16f;
    }

    thresholdDb = juce::jlimit(-60.0f, 0.0f, thresholdDb);

    const auto attackCoefficient = coefficientForTime(sampleRate, attackMs * 0.001f);
    const auto releaseCoefficient = coefficientForTime(sampleRate, releaseMs * 0.001f);
    const auto ratio = getRatio();
    const auto ratioScale = ratioAudibilityScale(ratio);
    const auto noiseMode = getNoiseMode();
    auto blockGainReductionDb = 0.0f;
    auto blockInputPeak = 0.0f;
    auto blockOutputPeak = 0.0f;

    for (auto sample = 0; sample < numSamples; ++sample)
    {
        const auto currentInputGain = inputGain.getNextValue();
        const auto currentOutputGain = outputGain.getNextValue();
        const auto currentMix = mix.getNextValue();

        auto detectorLevel = 0.0f;

        for (auto channel = 0; channel < bufferChannels; ++channel)
        {
            const auto sidechainChannel = juce::jmin(channel, maxSidechainChannels - 1);
            const auto rawDetectorSample = dryBuffer.getReadPointer(channel)[sample] * currentInputGain;
            const auto detectorSample = std::isfinite(rawDetectorSample) ? rawDetectorSample : 0.0f;
            blockInputPeak = juce::jmax(blockInputPeak, std::abs(detectorSample));
            detectorLevel = juce::jmax(detectorLevel, std::abs(processSidechainHighPass(detectorSample, sidechainChannel)));
        }

        const auto envelopeCoefficient = detectorLevel > detectorEnvelope ? attackCoefficient : releaseCoefficient;
        detectorEnvelope = envelopeCoefficient * detectorEnvelope + (1.0f - envelopeCoefficient) * detectorLevel;

        const auto detectorDb = juce::Decibels::gainToDecibels(detectorEnvelope, minDetectorDb);
        const auto overDb = detectorDb - thresholdDb;
        auto currentGainReductionDb = 0.0f;

        if (overDb > 0.0f)
            currentGainReductionDb = overDb * (1.0f - 1.0f / ratio) * gainReductionScale * ratioScale;

        currentGainReductionDb = juce::jlimit(0.0f, 30.0f, currentGainReductionDb);
        blockGainReductionDb = juce::jmax(blockGainReductionDb, currentGainReductionDb);

        const auto autoMakeupDb = juce::jlimit(0.0f, 8.0f, currentGainReductionDb * 0.28f);
        const auto compressorGain = juce::Decibels::decibelsToGain(-currentGainReductionDb + autoMakeupDb);
        const auto noise = getNoiseSample(noiseMode);

        for (auto channel = 0; channel < bufferChannels; ++channel)
        {
            auto* samples = buffer.getWritePointer(channel);
            const auto dry = dryBuffer.getReadPointer(channel)[sample];
            auto wet = dry * currentInputGain * compressorGain;

            if (revision == Revision::black)
                wet *= 0.985f;

            if (revision == Revision::hot)
            {
                const auto drive = 1.0f + saturationAmount * 2.5f;
                wet = (1.0f - saturationAmount) * wet + saturationAmount * std::tanh(wet * drive) / std::tanh(drive);
            }

            wet = (wet + noise) * currentOutputGain;

            if (! std::isfinite(wet))
                wet = 0.0f;

            if (std::abs(wet) > 3.0f)
                wet = std::tanh(wet / 3.0f) * 3.0f;

            const auto outputSample = dry * (1.0f - currentMix) + wet * currentMix;
            samples[sample] = std::isfinite(outputSample) ? outputSample : 0.0f;
            blockOutputPeak = juce::jmax(blockOutputPeak, std::abs(samples[sample]));
        }
    }

    displayedGainReductionDb = blockGainReductionDb > displayedGainReductionDb
                                   ? blockGainReductionDb
                                   : displayedGainReductionDb * 0.9f + blockGainReductionDb * 0.1f;
    gainReductionDb.store(displayedGainReductionDb);
    inputPeak.store(blockInputPeak);
    outputPeak.store(blockOutputPeak);

    sanitizeBuffer(buffer);
}

float FETCompressorModule::getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept
{
    if (parameter == nullptr)
        return fallback;

    const auto value = parameter->load();
    return std::isfinite(value) ? value : fallback;
}

int FETCompressorModule::getChoiceIndex(std::atomic<float>* parameter, int fallback, int maxIndex) const noexcept
{
    return juce::jlimit(0, maxIndex, juce::roundToInt(getParameterValue(parameter, static_cast<float>(fallback))));
}

float FETCompressorModule::getRatio() const noexcept
{
    switch (getChoiceIndex(parameters.compRatio, 0, 4))
    {
        case 1: return 8.0f;
        case 2: return 12.0f;
        case 3: return 20.0f;
        case 4: return 30.0f;
        default: return 4.0f;
    }
}

FETCompressorModule::Revision FETCompressorModule::getRevision() const noexcept
{
    switch (getChoiceIndex(parameters.compRevision, 0, 3))
    {
        case 1: return Revision::blue;
        case 2: return Revision::black;
        case 3: return Revision::hot;
        default: return Revision::clean;
    }
}

FETCompressorModule::NoiseMode FETCompressorModule::getNoiseMode() const noexcept
{
    switch (getChoiceIndex(parameters.compNoiseMode, 0, 2))
    {
        case 1: return NoiseMode::low;
        case 2: return NoiseMode::high;
        default: return NoiseMode::off;
    }
}

void FETCompressorModule::updateSidechainFilter(float frequencyHz) noexcept
{
    const auto safeFrequency = juce::jlimit(20.0f,
                                           static_cast<float>(sampleRate * 0.45),
                                           std::isfinite(frequencyHz) ? frequencyHz : 90.0f);

    if (std::abs(safeFrequency - cachedSidechainHpHz) < 0.001f)
        return;

    const auto rc = 1.0f / (2.0f * juce::MathConstants<float>::pi * safeFrequency);
    const auto dt = 1.0f / static_cast<float>(sampleRate);
    sidechainAlpha = rc / (rc + dt);
    cachedSidechainHpHz = safeFrequency;
}

float FETCompressorModule::processSidechainHighPass(float input, int channel) noexcept
{
    const auto index = static_cast<size_t>(juce::jlimit(0, maxSidechainChannels - 1, channel));
    const auto output = sidechainAlpha * (sidechainY1[index] + input - sidechainX1[index]);

    sidechainX1[index] = input;
    sidechainY1[index] = std::isfinite(output) ? output : 0.0f;

    return sidechainY1[index];
}

float FETCompressorModule::getNoiseSample(NoiseMode mode) noexcept
{
    if (mode == NoiseMode::off)
        return 0.0f;

    noiseState = noiseState * 1664525u + 1013904223u;
    const auto normalised = static_cast<float>((noiseState >> 8) & 0x00ffffff) / 8388607.5f - 1.0f;
    const auto gain = mode == NoiseMode::low ? 0.0000056f : 0.000022f;

    return normalised * gain;
}

void FETCompressorModule::storeBypassedMeterLevels(const juce::AudioBuffer<float>& buffer) noexcept
{
    auto peak = 0.0f;

    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        const auto* samples = buffer.getReadPointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            if (std::isfinite(samples[sample]))
                peak = juce::jmax(peak, std::abs(samples[sample]));
    }

    inputPeak.store(peak);
    outputPeak.store(peak);
}

void FETCompressorModule::sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept
{
    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            if (! std::isfinite(samples[sample]))
                samples[sample] = 0.0f;
    }
}
