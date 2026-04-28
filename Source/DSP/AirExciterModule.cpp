#include "AirExciterModule.h"

#include <cmath>

namespace
{
    constexpr auto filterQ = 0.70710678f;
    constexpr auto midBandQ = 1.05f;
    constexpr auto sibilanceQ = 1.65f;

    float coefficientForTime(double sampleRate, float timeSeconds) noexcept
    {
        if (sampleRate <= 0.0 || timeSeconds <= 0.0f)
            return 0.0f;

        return std::exp(-1.0f / static_cast<float>(sampleRate * static_cast<double>(timeSeconds)));
    }
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

    midBandFilter.prepare(spec);
    highPassFilter.prepare(spec);
    toneShelfFilter.prepare(spec);
    sibilanceFilter.prepare(spec);

    dryBuffer.setSize(static_cast<int>(spec.numChannels),
                      static_cast<int>(spec.maximumBlockSize),
                      false,
                      false,
                      true);
    midBuffer.setSize(static_cast<int>(spec.numChannels),
                      static_cast<int>(spec.maximumBlockSize),
                      false,
                      false,
                      true);
    highBuffer.setSize(static_cast<int>(spec.numChannels),
                       static_cast<int>(spec.maximumBlockSize),
                       false,
                       false,
                       true);
    sibilanceBuffer.setSize(static_cast<int>(spec.numChannels),
                            static_cast<int>(spec.maximumBlockSize),
                            false,
                            false,
                            true);

    amount.reset(spec.sampleRate, 0.02);
    midAir.reset(spec.sampleRate, 0.02);
    highAir.reset(spec.sampleRate, 0.02);
    drive.reset(spec.sampleRate, 0.02);
    density.reset(spec.sampleRate, 0.02);
    dynamic.reset(spec.sampleRate, 0.02);
    deEss.reset(spec.sampleRate, 0.02);
    outputGain.reset(spec.sampleRate, 0.02);
    mix.reset(spec.sampleRate, 0.02);

    amount.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airAmount, 0.0f)));
    midAir.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airMidAir, 0.55f)));
    highAir.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airHighAir, 0.65f)));
    drive.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDrive, 0.0f)));
    density.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDensity, 0.4f)));
    dynamic.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDynamic, 0.35f)));
    deEss.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDeEss, 0.0f)));
    outputGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(getParameterValue(parameters.airOutputDb, 0.0f)));
    mix.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airMix, 1.0f)));

    cached = {};
    updateFilterCoefficients(true);
    reset();
}

void AirExciterModule::reset()
{
    midBandFilter.reset();
    highPassFilter.reset();
    toneShelfFilter.reset();
    sibilanceFilter.reset();
    dynamicEnvelope = 0.0f;
    sibilanceEnvelope = 0.0f;
}

void AirExciterModule::process(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    if (! prepared || sampleRate <= 0.0)
        return;

    const auto enabled = getParameterValue(parameters.airEnabled, 1.0f) > 0.5f;
    const auto targetMix = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airMix, 1.0f));

    if (! enabled || targetMix <= 0.0001f)
    {
        dynamicEnvelope = 0.0f;
        sibilanceEnvelope = 0.0f;
        return;
    }

    const auto bufferChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (bufferChannels <= 0 || numSamples <= 0
        || bufferChannels > dryBuffer.getNumChannels()
        || numSamples > dryBuffer.getNumSamples())
    {
        return;
    }

    amount.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airAmount, 0.0f)));
    midAir.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airMidAir, 0.55f)));
    highAir.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airHighAir, 0.65f)));
    drive.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDrive, 0.0f)));
    density.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDensity, 0.4f)));
    dynamic.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDynamic, 0.35f)));
    deEss.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airDeEss, 0.0f)));
    outputGain.setTargetValue(juce::Decibels::decibelsToGain(juce::jlimit(-12.0f, 12.0f, getParameterValue(parameters.airOutputDb, 0.0f))));
    mix.setTargetValue(targetMix);
    updateFilterCoefficients(false);

    for (auto channel = 0; channel < bufferChannels; ++channel)
    {
        dryBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
        midBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
        highBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
        sibilanceBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    }

    auto midBlock = juce::dsp::AudioBlock<float>(midBuffer)
                        .getSubsetChannelBlock(0, static_cast<size_t>(bufferChannels))
                        .getSubBlock(0, static_cast<size_t>(numSamples));
    auto highBlock = juce::dsp::AudioBlock<float>(highBuffer)
                         .getSubsetChannelBlock(0, static_cast<size_t>(bufferChannels))
                         .getSubBlock(0, static_cast<size_t>(numSamples));
    auto sibilanceBlock = juce::dsp::AudioBlock<float>(sibilanceBuffer)
                              .getSubsetChannelBlock(0, static_cast<size_t>(bufferChannels))
                              .getSubBlock(0, static_cast<size_t>(numSamples));

    auto midContext = juce::dsp::ProcessContextReplacing<float>(midBlock);
    auto highContext = juce::dsp::ProcessContextReplacing<float>(highBlock);
    auto sibilanceContext = juce::dsp::ProcessContextReplacing<float>(sibilanceBlock);

    midBandFilter.process(midContext);
    highPassFilter.process(highContext);
    toneShelfFilter.process(highContext);
    sibilanceFilter.process(sibilanceContext);

    const auto dynamicAttack = coefficientForTime(sampleRate, 0.002f);
    const auto dynamicRelease = coefficientForTime(sampleRate, 0.14f);
    const auto sibilanceAttack = coefficientForTime(sampleRate, 0.0007f);
    const auto sibilanceRelease = coefficientForTime(sampleRate, 0.06f);

    for (auto sample = 0; sample < numSamples; ++sample)
    {
        auto linkedHighEnergy = 0.0f;
        auto linkedSibilanceEnergy = 0.0f;

        for (auto channel = 0; channel < bufferChannels; ++channel)
        {
            linkedHighEnergy = juce::jmax(linkedHighEnergy, std::abs(highBuffer.getReadPointer(channel)[sample]));
            linkedSibilanceEnergy = juce::jmax(linkedSibilanceEnergy, std::abs(sibilanceBuffer.getReadPointer(channel)[sample]));
        }

        const auto dynamicCoefficient = linkedHighEnergy > dynamicEnvelope ? dynamicAttack : dynamicRelease;
        dynamicEnvelope = dynamicCoefficient * dynamicEnvelope + (1.0f - dynamicCoefficient) * linkedHighEnergy;

        const auto sibilanceCoefficient = linkedSibilanceEnergy > sibilanceEnvelope ? sibilanceAttack : sibilanceRelease;
        sibilanceEnvelope = sibilanceCoefficient * sibilanceEnvelope + (1.0f - sibilanceCoefficient) * linkedSibilanceEnergy;

        const auto currentAmount = amount.getNextValue();
        const auto currentMidAir = midAir.getNextValue();
        const auto currentHighAir = highAir.getNextValue();
        const auto currentDrive = drive.getNextValue();
        const auto currentDensity = density.getNextValue();
        const auto currentDynamic = dynamic.getNextValue();
        const auto currentDeEss = deEss.getNextValue();
        const auto currentOutputGain = outputGain.getNextValue();
        const auto currentMix = mix.getNextValue();

        const auto excitationBase = juce::jmax(currentAmount, currentDrive * 0.22f);
        const auto dynamicDb = juce::Decibels::gainToDecibels(dynamicEnvelope, -90.0f);
        const auto dynamicNormalised = juce::jlimit(0.0f, 1.0f, (dynamicDb + 58.0f) / 48.0f);
        const auto dynamicFactor = (1.0f - currentDynamic) + currentDynamic * (0.18f + dynamicNormalised * 0.82f);

        const auto sibilanceDb = juce::Decibels::gainToDecibels(sibilanceEnvelope, -90.0f);
        const auto sibilanceOverDb = juce::jmax(0.0f, sibilanceDb + 28.0f);
        const auto deEssReductionDb = juce::jlimit(0.0f, 12.0f, sibilanceOverDb * currentDeEss * (0.45f + currentHighAir * 0.55f));
        const auto deEssGain = juce::Decibels::decibelsToGain(-deEssReductionDb);

        const auto harmonicDrive = 1.0f + currentDrive * 7.0f + currentDensity * 4.0f;
        const auto harmonicNormaliser = 1.0f / std::tanh(harmonicDrive);
        const auto densityWeight = 0.35f + currentDensity * 1.65f;
        const auto midBlend = excitationBase * currentMidAir * dynamicFactor * (0.26f + currentDensity * 0.42f);
        const auto highBlend = excitationBase * currentHighAir * dynamicFactor * deEssGain * (0.20f + currentDensity * 0.34f);
        const auto harmonicBlend = excitationBase * densityWeight * dynamicFactor * deEssGain * (0.10f + currentDrive * 0.72f);
        const auto safetyTrim = 1.0f - excitationBase * 0.04f;

        for (auto channel = 0; channel < bufferChannels; ++channel)
        {
            auto* samples = buffer.getWritePointer(channel);
            const auto dry = dryBuffer.getReadPointer(channel)[sample];
            const auto mid = midBuffer.getReadPointer(channel)[sample];
            const auto high = highBuffer.getReadPointer(channel)[sample];
            const auto safeDry = std::isfinite(dry) ? dry : 0.0f;
            const auto safeMid = std::isfinite(mid) ? mid : 0.0f;
            const auto safeHigh = std::isfinite(high) ? high : 0.0f;
            const auto harmonicSource = safeHigh * 0.72f + safeMid * 0.38f;
            const auto saturated = std::tanh(harmonicSource * harmonicDrive) * harmonicNormaliser;
            const auto harmonics = saturated - harmonicSource;

            auto wet = safeDry
                     + safeMid * midBlend
                     + safeHigh * highBlend
                     + harmonics * harmonicBlend;

            wet *= currentOutputGain * safetyTrim;

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
    const auto midFrequency = juce::jlimit(3000.0f, 8000.0f, frequency * 0.68f);
    const auto sibilanceFrequency = juce::jlimit(5500.0f, 9000.0f, frequency * 0.92f);
    const auto tone = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.airTone, 0.5f));
    const auto frequencyChanged = frequency != cached.frequencyHz;
    const auto midFrequencyChanged = midFrequency != cached.midFrequencyHz;
    const auto sibilanceFrequencyChanged = sibilanceFrequency != cached.sibilanceFrequencyHz;

    if (force || midFrequencyChanged)
    {
        *midBandFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, midFrequency, midBandQ);
        cached.midFrequencyHz = midFrequency;
    }

    if (force || frequencyChanged)
    {
        *highPassFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeHighPass(sampleRate, frequency, filterQ);
        cached.frequencyHz = frequency;
    }

    if (force || sibilanceFrequencyChanged)
    {
        *sibilanceFilter.state = juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, sibilanceFrequency, sibilanceQ);
        cached.sibilanceFrequencyHz = sibilanceFrequency;
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
