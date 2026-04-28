#include "DelayModule.h"

#include <cmath>

namespace
{
    constexpr auto internalDelayChannels = 2;
    constexpr auto maxDelaySeconds = 4.0;

    float coefficientForTime(double sampleRate, float timeSeconds) noexcept
    {
        if (sampleRate <= 0.0 || timeSeconds <= 0.0f)
            return 0.0f;

        return std::exp(-1.0f / static_cast<float>(sampleRate * static_cast<double>(timeSeconds)));
    }
}

void DelayModule::setParameters(ParameterPointers newParameters) noexcept
{
    parameters = newParameters;
}

void DelayModule::setHostBpm(double bpm) noexcept
{
    if (std::isfinite(bpm))
        hostBpm = juce::jlimit(20.0, 300.0, bpm);
}

void DelayModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    AudioModule::prepare(spec);

    prepared = spec.sampleRate > 0.0 && spec.maximumBlockSize > 0;

    if (! prepared)
        return;

    delayBufferSize = static_cast<int>(std::ceil(spec.sampleRate * maxDelaySeconds)) + 8;
    delayBuffer.setSize(internalDelayChannels, delayBufferSize, false, false, true);

    leftDelaySamples.reset(spec.sampleRate, 0.04);
    rightDelaySamples.reset(spec.sampleRate, 0.04);
    feedback.reset(spec.sampleRate, 0.02);
    send.reset(spec.sampleRate, 0.02);
    stereoWidth.reset(spec.sampleRate, 0.02);
    loFi.reset(spec.sampleRate, 0.02);
    modDepth.reset(spec.sampleRate, 0.02);
    ducking.reset(spec.sampleRate, 0.02);

    const auto initialDelaySamples = static_cast<float>(sampleRate * 0.35);
    leftDelaySamples.setCurrentAndTargetValue(initialDelaySamples);
    rightDelaySamples.setCurrentAndTargetValue(initialDelaySamples);
    feedback.setCurrentAndTargetValue(juce::jlimit(0.0f, 0.92f, getParameterValue(parameters.delayFeedback, 0.25f) * 0.92f));
    send.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delaySend, 0.0f)));
    stereoWidth.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayStereoWidth, 1.0f)));
    loFi.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayLoFi, 0.0f)));
    modDepth.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayModDepth, 0.0f)));
    ducking.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayDucking, 0.0f)));

    reset();
    updateFilterCoefficients(true);
}

void DelayModule::reset()
{
    delayBuffer.clear();
    writePosition = 0;
    highPassX1 = {};
    highPassY1 = {};
    lowPassY1 = {};
    duckEnvelope = 0.0f;
    modPhase = 0.0f;
}

void DelayModule::process(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    if (! prepared || sampleRate <= 0.0 || delayBufferSize <= 8)
        return;

    const auto enabled = getParameterValue(parameters.delayEnabled, 1.0f) > 0.5f;
    const auto targetSend = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delaySend, 0.0f));

    if (! enabled || targetSend <= 0.0001f)
        return;

    const auto bufferChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (bufferChannels <= 0 || numSamples <= 0)
        return;

    updateFilterCoefficients(false);

    const auto syncEnabled = getParameterValue(parameters.delaySyncEnabled, 0.0f) > 0.5f;
    const auto linkEnabled = getParameterValue(parameters.delayLink, 1.0f) > 0.5f;
    const auto mode = getMode();
    const auto freezeEnabled = getParameterValue(parameters.delayFreeze, 0.0f) > 0.5f;
    const auto baseDelayMs = syncEnabled
                                 ? getSyncedDelayMs()
                                 : juce::jlimit(20.0f, 2000.0f, getParameterValue(parameters.delayTimeMs, 350.0f));

    auto leftDelayMs = baseDelayMs;
    auto rightDelayMs = baseDelayMs;

    if (! syncEnabled && mode == Mode::dual && ! linkEnabled)
    {
        leftDelayMs = juce::jlimit(20.0f, 2000.0f, getParameterValue(parameters.delayLeftTime, baseDelayMs));
        rightDelayMs = juce::jlimit(20.0f, 2000.0f, getParameterValue(parameters.delayRightTime, baseDelayMs * 1.5f));
    }

    const auto maxDelaySamples = static_cast<float>(delayBufferSize - 4);
    leftDelaySamples.setTargetValue(juce::jlimit(1.0f, maxDelaySamples, static_cast<float>(sampleRate * leftDelayMs * 0.001)));
    rightDelaySamples.setTargetValue(juce::jlimit(1.0f, maxDelaySamples, static_cast<float>(sampleRate * rightDelayMs * 0.001)));
    feedback.setTargetValue(freezeEnabled ? 0.985f
                                          : juce::jlimit(0.0f, 0.92f, getParameterValue(parameters.delayFeedback, 0.25f) * 0.92f));
    send.setTargetValue(targetSend);
    stereoWidth.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayStereoWidth, 1.0f)));
    loFi.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayLoFi, 0.0f)));
    modDepth.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayModDepth, 0.0f)));
    ducking.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayDucking, 0.0f)));

    const auto modRateHz = juce::jlimit(0.05f, 8.0f, getParameterValue(parameters.delayModRate, 0.35f));
    const auto duckAttack = coefficientForTime(sampleRate, 0.004f);
    const auto duckRelease = coefficientForTime(sampleRate, 0.18f);

    for (auto sample = 0; sample < numSamples; ++sample)
    {
        const auto dryLeft = std::isfinite(buffer.getReadPointer(0)[sample]) ? buffer.getReadPointer(0)[sample] : 0.0f;
        const auto dryRight = bufferChannels > 1 && std::isfinite(buffer.getReadPointer(1)[sample])
                                  ? buffer.getReadPointer(1)[sample]
                                  : dryLeft;

        const auto currentModDepth = modDepth.getNextValue();
        const auto modulationSamples = currentModDepth * static_cast<float>(sampleRate * 0.025);
        const auto leftMod = std::sin(modPhase) * modulationSamples;
        const auto rightMod = std::sin(modPhase + juce::MathConstants<float>::halfPi) * modulationSamples;

        auto wetLeft = processWetFilters(readDelaySample(0, leftDelaySamples.getNextValue() + leftMod), 0);
        auto wetRight = processWetFilters(readDelaySample(1, rightDelaySamples.getNextValue() + rightMod), 1);

        const auto currentLoFi = loFi.getNextValue();
        wetLeft = applyLoFi(wetLeft, currentLoFi);
        wetRight = applyLoFi(wetRight, currentLoFi);

        const auto inputLevel = juce::jmax(std::abs(dryLeft), std::abs(dryRight));
        const auto duckCoefficient = inputLevel > duckEnvelope ? duckAttack : duckRelease;
        duckEnvelope = duckCoefficient * duckEnvelope + (1.0f - duckCoefficient) * inputLevel;

        const auto currentDucking = ducking.getNextValue();
        const auto duckDb = juce::jlimit(0.0f, 18.0f, currentDucking * 18.0f * juce::jlimit(0.0f, 1.0f, (juce::Decibels::gainToDecibels(duckEnvelope, -90.0f) + 42.0f) / 42.0f));
        const auto duckGain = juce::Decibels::decibelsToGain(-duckDb);
        const auto currentSend = send.getNextValue();
        const auto currentWidth = stereoWidth.getNextValue();

        if (bufferChannels > 1)
        {
            const auto wetMid = (wetLeft + wetRight) * 0.5f;
            const auto wetSide = (wetLeft - wetRight) * 0.5f * currentWidth;
            wetLeft = wetMid + wetSide;
            wetRight = wetMid - wetSide;
        }

        auto* leftSamples = buffer.getWritePointer(0);
        leftSamples[sample] = sanitizeSample(dryLeft + wetLeft * currentSend * duckGain);

        if (bufferChannels > 1)
        {
            auto* rightSamples = buffer.getWritePointer(1);
            rightSamples[sample] = sanitizeSample(dryRight + wetRight * currentSend * duckGain);
        }

        const auto currentFeedback = feedback.getNextValue();
        const auto inputFeed = freezeEnabled ? 0.0f : 1.0f;
        auto writeLeft = 0.0f;
        auto writeRight = 0.0f;

        if (mode == Mode::pingPong)
        {
            const auto monoInput = (dryLeft + dryRight) * 0.5f;
            writeLeft = monoInput * inputFeed + wetRight * currentFeedback;
            writeRight = wetLeft * currentFeedback;
        }
        else if (mode == Mode::dual)
        {
            writeLeft = dryLeft * inputFeed + wetLeft * currentFeedback;
            writeRight = dryRight * inputFeed + wetRight * currentFeedback;
        }
        else
        {
            const auto monoInput = (dryLeft + dryRight) * 0.5f;
            const auto monoWet = (wetLeft + wetRight) * 0.5f;
            writeLeft = monoInput * inputFeed + monoWet * currentFeedback;
            writeRight = writeLeft;
        }

        delayBuffer.setSample(0, writePosition, sanitizeSample(writeLeft));
        delayBuffer.setSample(1, writePosition, sanitizeSample(writeRight));

        if (++writePosition >= delayBufferSize)
            writePosition = 0;

        modPhase += juce::MathConstants<float>::twoPi * modRateHz / static_cast<float>(sampleRate);
        if (modPhase >= juce::MathConstants<float>::twoPi)
            modPhase -= juce::MathConstants<float>::twoPi;
    }

    sanitizeBuffer(buffer);
}

float DelayModule::getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept
{
    if (parameter == nullptr)
        return fallback;

    const auto value = parameter->load();
    return std::isfinite(value) ? value : fallback;
}

int DelayModule::getChoiceIndex(std::atomic<float>* parameter, int fallback, int maxIndex) const noexcept
{
    return juce::jlimit(0, maxIndex, juce::roundToInt(getParameterValue(parameter, static_cast<float>(fallback))));
}

DelayModule::Mode DelayModule::getMode() const noexcept
{
    switch (getChoiceIndex(parameters.delayMode, 0, 2))
    {
        case 1: return Mode::pingPong;
        case 2: return Mode::dual;
        default: return Mode::mono;
    }
}

float DelayModule::getSyncedDelayMs() const noexcept
{
    const auto quarterNoteMs = static_cast<float>(60000.0 / juce::jlimit(20.0, 300.0, hostBpm));

    switch (getChoiceIndex(parameters.delayNoteDivision, 0, 7))
    {
        case 1: return quarterNoteMs * 0.5f;
        case 2: return quarterNoteMs * 0.75f;
        case 3: return quarterNoteMs / 3.0f;
        case 4: return quarterNoteMs * 0.25f;
        case 5: return quarterNoteMs * 0.375f;
        case 6: return quarterNoteMs / 6.0f;
        case 7: return quarterNoteMs * 2.0f;
        default: return quarterNoteMs;
    }
}

void DelayModule::updateFilterCoefficients(bool force) noexcept
{
    const auto loFiAmount = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.delayLoFi, 0.0f));
    const auto highPass = juce::jlimit(20.0f, 1000.0f, getParameterValue(parameters.delayHighPassHz, 20.0f));
    const auto lowPassBase = juce::jlimit(1000.0f, static_cast<float>(sampleRate * 0.45), getParameterValue(parameters.delayLowPassHz, 16000.0f));
    const auto lowPass = juce::jlimit(1000.0f, static_cast<float>(sampleRate * 0.45), lowPassBase * (1.0f - loFiAmount * 0.62f));

    if (! force
        && highPass == cachedFilters.highPassHz
        && lowPass == cachedFilters.lowPassHz
        && loFiAmount == cachedFilters.loFi)
    {
        return;
    }

    const auto dt = 1.0f / static_cast<float>(sampleRate);
    const auto hpRc = 1.0f / (juce::MathConstants<float>::twoPi * highPass);
    const auto lpRc = 1.0f / (juce::MathConstants<float>::twoPi * lowPass);

    highPassAlpha = hpRc / (hpRc + dt);
    lowPassCoefficient = dt / (lpRc + dt);

    cachedFilters.highPassHz = highPass;
    cachedFilters.lowPassHz = lowPass;
    cachedFilters.loFi = loFiAmount;
}

float DelayModule::readDelaySample(int channel, float delaySamples) const noexcept
{
    const auto safeDelay = juce::jlimit(1.0f, static_cast<float>(delayBufferSize - 3), std::isfinite(delaySamples) ? delaySamples : 1.0f);
    auto readPosition = static_cast<float>(writePosition) - safeDelay;

    while (readPosition < 0.0f)
        readPosition += static_cast<float>(delayBufferSize);

    const auto index0 = juce::jlimit(0, delayBufferSize - 1, static_cast<int>(readPosition));
    const auto index1 = (index0 + 1) % delayBufferSize;
    const auto fraction = readPosition - static_cast<float>(index0);
    const auto* data = delayBuffer.getReadPointer(juce::jlimit(0, internalDelayChannels - 1, channel));

    return data[index0] + (data[index1] - data[index0]) * fraction;
}

float DelayModule::processWetFilters(float input, int channel) noexcept
{
    const auto index = static_cast<size_t>(juce::jlimit(0, internalDelayChannels - 1, channel));
    const auto safeInput = std::isfinite(input) ? input : 0.0f;
    const auto highPassed = highPassAlpha * (highPassY1[index] + safeInput - highPassX1[index]);
    highPassX1[index] = safeInput;
    highPassY1[index] = std::isfinite(highPassed) ? highPassed : 0.0f;

    lowPassY1[index] += lowPassCoefficient * (highPassY1[index] - lowPassY1[index]);
    return std::isfinite(lowPassY1[index]) ? lowPassY1[index] : 0.0f;
}

float DelayModule::applyLoFi(float input, float amount) const noexcept
{
    amount = juce::jlimit(0.0f, 1.0f, std::isfinite(amount) ? amount : 0.0f);

    if (amount <= 0.0001f)
        return input;

    const auto levels = juce::jmax(16.0f, 8192.0f * (1.0f - amount) + 32.0f * amount);
    const auto quantized = std::round(input * levels) / levels;
    const auto softened = std::tanh(quantized * (1.0f + amount * 1.5f)) / std::tanh(1.0f + amount * 1.5f);

    return input * (1.0f - amount) + softened * amount;
}

float DelayModule::sanitizeSample(float value) const noexcept
{
    if (! std::isfinite(value))
        return 0.0f;

    if (std::abs(value) > 3.0f)
        return std::tanh(value / 3.0f) * 3.0f;

    return value;
}

void DelayModule::sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept
{
    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            if (! std::isfinite(samples[sample]))
                samples[sample] = 0.0f;
    }
}
