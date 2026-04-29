#include "ReverbModule.h"

#include <cmath>

namespace
{
    constexpr auto maxPredelaySeconds = 0.35;
    constexpr auto maxTankSeconds = 2.0;

    float coefficientForTime(double sampleRate, float timeSeconds) noexcept
    {
        if (sampleRate <= 0.0 || timeSeconds <= 0.0f)
            return 0.0f;

        return std::exp(-1.0f / static_cast<float>(sampleRate * static_cast<double>(timeSeconds)));
    }

    float lowPassCoefficient(double sampleRate, float cutoffHz) noexcept
    {
        if (sampleRate <= 0.0 || cutoffHz <= 0.0f)
            return 1.0f;

        const auto dt = 1.0f / static_cast<float>(sampleRate);
        const auto rc = 1.0f / (juce::MathConstants<float>::twoPi * cutoffHz);
        return dt / (rc + dt);
    }

    float highPassAlpha(double sampleRate, float cutoffHz) noexcept
    {
        if (sampleRate <= 0.0 || cutoffHz <= 0.0f)
            return 0.0f;

        const auto dt = 1.0f / static_cast<float>(sampleRate);
        const auto rc = 1.0f / (juce::MathConstants<float>::twoPi * cutoffHz);
        return rc / (rc + dt);
    }
}

void ReverbModule::setParameters(ParameterPointers newParameters) noexcept
{
    parameters = newParameters;
}

void ReverbModule::setHostBpm(double bpm) noexcept
{
    if (std::isfinite(bpm))
        hostBpm = juce::jlimit(20.0, 300.0, bpm);
}

void ReverbModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    AudioModule::prepare(spec);

    prepared = spec.sampleRate > 0.0 && spec.maximumBlockSize > 0;

    if (! prepared)
        return;

    predelayBufferSize = static_cast<int>(std::ceil(spec.sampleRate * maxPredelaySeconds)) + 8;
    tankBufferSize = static_cast<int>(std::ceil(spec.sampleRate * maxTankSeconds)) + 8;
    predelayBuffer.setSize(internalChannels, predelayBufferSize, false, false, true);
    tankBuffer.setSize(tankLineCount, tankBufferSize, false, false, true);

    mix.reset(spec.sampleRate, 0.025);
    predelaySamples.reset(spec.sampleRate, 0.035);
    width.reset(spec.sampleRate, 0.025);
    ducking.reset(spec.sampleRate, 0.025);

    mix.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbMix, 0.0f)));
    predelaySamples.setCurrentAndTargetValue(juce::jlimit(1.0f, static_cast<float>(predelayBufferSize - 4),
                                                         static_cast<float>(sampleRate * getPredelayMs() * 0.001)));
    width.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbWidth, 1.0f)));
    ducking.setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbDucking, 0.0f)));

    reset();
}

void ReverbModule::reset()
{
    predelayBuffer.clear();
    tankBuffer.clear();
    predelayWritePosition = 0;
    tankWritePosition = 0;
    feedbackHighPassX1 = {};
    feedbackHighPassY1 = {};
    feedbackLowPassY1 = {};
    wetHighPassX1 = {};
    wetHighPassY1 = {};
    wetLowPassY1 = {};
    monoBassLowPass = {};
    modPhase = 0.0f;
    duckEnvelope = 0.0f;
    attackEnvelope = 0.0f;
}

void ReverbModule::process(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    if (! prepared || sampleRate <= 0.0 || predelayBufferSize <= 8 || tankBufferSize <= 8)
        return;

    const auto enabled = getParameterValue(parameters.reverbEnabled, 1.0f) > 0.5f;
    const auto targetMix = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbMix, 0.0f));

    if (! enabled || targetMix <= 0.0001f)
    {
        mix.setCurrentAndTargetValue(0.0f);
        return;
    }

    const auto bufferChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (bufferChannels <= 0 || numSamples <= 0)
        return;

    const auto mode = getMode();
    const auto color = getColor();
    const auto modeSettings = getModeSettings(mode);
    const auto colorSettings = getColorSettings(color);
    const auto size = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbSize, 0.50f));
    const auto decaySeconds = juce::jlimit(0.3f, 18.0f, getParameterValue(parameters.reverbDecaySec, 1.85f) * modeSettings.decayScale);
    const auto earlyAmount = juce::jlimit(0.0f, 1.4f, getParameterValue(parameters.reverbEarly, 0.55f) * modeSettings.earlyScale);
    const auto lateAmount = juce::jlimit(0.0f, 1.4f, getParameterValue(parameters.reverbLate, 0.68f) * modeSettings.lateScale);
    const auto diffEarly = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbDiffusionEarly, 0.38f));
    const auto diffLate = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbDiffusionLate, 0.72f));
    const auto freezeEnabled = getParameterValue(parameters.reverbFreeze, 0.0f) > 0.5f;
    const auto monoBassEnabled = getParameterValue(parameters.reverbMonoBass, 0.0f) > 0.5f;
    const auto attack = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbAttack, 0.0f));
    const auto lowCut = juce::jlimit(20.0f, 1000.0f, getParameterValue(parameters.reverbLowCutHz, 160.0f));
    const auto highCut = juce::jlimit(1000.0f, static_cast<float>(sampleRate * 0.45),
                                      getParameterValue(parameters.reverbHighCutHz, 14000.0f) * colorSettings.brightness * modeSettings.brightness);
    const auto lowDamp = juce::jlimit(40.0f, 1000.0f, getParameterValue(parameters.reverbLowDampHz, 320.0f));
    const auto highDampDb = juce::jlimit(-24.0f, 0.0f, getParameterValue(parameters.reverbHighDampDb, -4.5f));
    const auto dampAmount = juce::jlimit(0.0f, 1.0f, -highDampDb / 24.0f);
    const auto feedbackLowPassHz = juce::jlimit(850.0f, static_cast<float>(sampleRate * 0.45),
                                                highCut * (1.0f - dampAmount * 0.60f) * colorSettings.damping);
    const auto wetHighPassAlpha = highPassAlpha(sampleRate, lowCut);
    const auto wetLowPassCoefficient = lowPassCoefficient(sampleRate, highCut);
    const auto feedbackHighPassAlpha = highPassAlpha(sampleRate, lowDamp);
    const auto feedbackLowPassCoefficient = lowPassCoefficient(sampleRate, feedbackLowPassHz);
    const auto monoBassCoefficient = lowPassCoefficient(sampleRate, 180.0f);
    const auto duckAttack = coefficientForTime(sampleRate, 0.012f);
    const auto duckRelease = coefficientForTime(sampleRate, 0.42f);
    const auto attackTimeSeconds = 0.004f + attack * 1.2f + modeSettings.attackBias;
    const auto attackCoefficient = coefficientForTime(sampleRate, attackTimeSeconds);
    const auto attackReleaseCoefficient = coefficientForTime(sampleRate, 1.4f);
    const auto modRate = juce::jlimit(0.05f, 5.0f, getParameterValue(parameters.reverbModRate, 0.25f));
    const auto modDepth = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbModDepth, 0.08f)) * colorSettings.modulation;
    const auto sizeScale = modeSettings.sizeScale * juce::jmap(size, 0.0f, 1.0f, 0.62f, 1.72f);
    const auto inputToTank = freezeEnabled ? 0.0f : 1.0f;
    const auto colorDrive = colorSettings.drive;
    const auto colorLoFi = colorSettings.loFi;

    mix.setTargetValue(targetMix);
    width.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbWidth, 1.0f)));
    ducking.setTargetValue(juce::jlimit(0.0f, 1.0f, getParameterValue(parameters.reverbDucking, 0.0f)));
    predelaySamples.setTargetValue(juce::jlimit(1.0f, static_cast<float>(predelayBufferSize - 4),
                                                static_cast<float>(sampleRate * getPredelayMs() * 0.001)));

    const std::array<float, tankLineCount> baseDelayMs { 31.1f, 43.7f, 59.3f, 73.1f, 89.9f, 107.3f, 127.7f, 149.3f };
    const std::array<float, tankLineCount> phaseOffsets {
        0.0f,
        0.73f,
        1.37f,
        2.11f,
        2.89f,
        3.41f,
        4.17f,
        5.02f
    };
    std::array<float, tankLineCount> tankDelaySamples {};
    std::array<float, tankLineCount> tankFeedback {};

    for (auto line = 0; line < tankLineCount; ++line)
    {
        const auto delayMs = juce::jlimit(8.0f, 320.0f, baseDelayMs[static_cast<size_t>(line)] * sizeScale);
        tankDelaySamples[static_cast<size_t>(line)] = juce::jlimit(1.0f, static_cast<float>(tankBufferSize - 4),
                                                                   static_cast<float>(sampleRate * delayMs * 0.001));

        const auto lineSeconds = delayMs * 0.001f;
        tankFeedback[static_cast<size_t>(line)] = freezeEnabled
                                                      ? 0.997f
                                                      : juce::jlimit(0.2f, 0.965f, std::pow(0.001f, lineSeconds / decaySeconds));
    }

    for (auto sample = 0; sample < numSamples; ++sample)
    {
        const auto dryLeft = sanitizeSample(buffer.getReadPointer(0)[sample]);
        const auto dryRight = bufferChannels > 1 ? sanitizeSample(buffer.getReadPointer(1)[sample]) : dryLeft;

        predelayBuffer.setSample(0, predelayWritePosition, dryLeft);
        predelayBuffer.setSample(1, predelayWritePosition, dryRight);

        const auto currentPredelaySamples = predelaySamples.getNextValue();
        const auto preLeft = readPredelaySample(0, currentPredelaySamples);
        const auto preRight = readPredelaySample(1, currentPredelaySamples);
        const auto tapScaleSamples = static_cast<float>(sampleRate * 0.001) * juce::jmap(size, 0.0f, 1.0f, 0.75f, 1.55f);

        auto earlyLeft = preLeft * 0.30f
                       + readPredelaySample(0, currentPredelaySamples + 8.0f * tapScaleSamples) * 0.28f
                       - readPredelaySample(1, currentPredelaySamples + 17.0f * tapScaleSamples) * 0.19f
                       + readPredelaySample(0, currentPredelaySamples + 29.0f * tapScaleSamples) * 0.16f
                       + readPredelaySample(1, currentPredelaySamples + 46.0f * tapScaleSamples) * 0.11f
                       - readPredelaySample(0, currentPredelaySamples + 71.0f * tapScaleSamples) * 0.07f;
        auto earlyRight = preRight * 0.30f
                        + readPredelaySample(1, currentPredelaySamples + 10.0f * tapScaleSamples) * 0.28f
                        - readPredelaySample(0, currentPredelaySamples + 21.0f * tapScaleSamples) * 0.19f
                        + readPredelaySample(1, currentPredelaySamples + 34.0f * tapScaleSamples) * 0.16f
                        + readPredelaySample(0, currentPredelaySamples + 53.0f * tapScaleSamples) * 0.11f
                        - readPredelaySample(1, currentPredelaySamples + 83.0f * tapScaleSamples) * 0.07f;

        const auto earlyFocus = 0.72f + (1.0f - diffEarly) * 0.26f;
        const auto earlySpread = juce::jlimit(0.25f, 1.25f, (0.64f + diffEarly * 0.54f + size * 0.18f) * modeSettings.widthScale);
        const auto earlyMid = (earlyLeft + earlyRight) * 0.5f;
        const auto earlySide = (earlyLeft - earlyRight) * 0.5f * earlySpread;
        earlyLeft = (earlyMid + earlySide) * earlyFocus;
        earlyRight = (earlyMid - earlySide) * earlyFocus;

        std::array<float, tankLineCount> tankOut {};
        auto tankSum = 0.0f;

        for (auto line = 0; line < tankLineCount; ++line)
        {
            const auto modulationSamples = modDepth * static_cast<float>(sampleRate * 0.0022)
                                         * std::sin(modPhase + phaseOffsets[static_cast<size_t>(line)]);
            tankOut[static_cast<size_t>(line)] = readTankSample(line,
                                                                tankDelaySamples[static_cast<size_t>(line)] + modulationSamples);
            tankSum += tankOut[static_cast<size_t>(line)];
        }

        const auto lateFocus = 0.70f + diffLate * 0.26f;
        const auto lateLeft = (tankOut[0] * 0.82f - tankOut[2] * 0.58f + tankOut[4] * 0.74f
                               - tankOut[5] * 0.30f + tankOut[6] * 0.42f)
                            * 0.27f * modeSettings.tailLevel * lateFocus;
        const auto lateRight = (tankOut[1] * 0.82f - tankOut[3] * 0.58f + tankOut[5] * 0.74f
                                - tankOut[4] * 0.30f + tankOut[7] * 0.42f)
                             * 0.27f * modeSettings.tailLevel * lateFocus;
        const auto monoPre = (preLeft + preRight) * 0.5f;
        const auto inputEnergy = juce::jmax(std::abs(dryLeft), std::abs(dryRight));
        const auto duckCoefficient = inputEnergy > duckEnvelope ? duckAttack : duckRelease;
        duckEnvelope = duckCoefficient * duckEnvelope + (1.0f - duckCoefficient) * inputEnergy;

        const auto tailEnergy = juce::jlimit(0.0f, 1.0f, std::abs(lateLeft) + std::abs(lateRight) + std::abs(earlyLeft) * 0.25f);
        const auto attackTarget = (inputEnergy + tailEnergy) > 0.00002f ? 1.0f : 0.0f;
        const auto currentAttackCoefficient = attackTarget > attackEnvelope ? attackCoefficient : attackReleaseCoefficient;
        attackEnvelope = currentAttackCoefficient * attackEnvelope + (1.0f - currentAttackCoefficient) * attackTarget;

        auto attackGain = attack <= 0.0001f ? 1.0f : attackEnvelope;
        if (mode == Mode::reverse)
            attackGain = std::pow(attackEnvelope, 1.8f);

        auto wetLeft = (earlyLeft * earlyAmount * 0.95f + lateLeft * lateAmount) * attackGain;
        auto wetRight = (earlyRight * earlyAmount * 0.95f + lateRight * lateAmount) * attackGain;

        wetLeft = processWetFilters(applyColor(wetLeft, colorDrive, colorLoFi), 0, wetHighPassAlpha, wetLowPassCoefficient);
        wetRight = processWetFilters(applyColor(wetRight, colorDrive, colorLoFi), 1, wetHighPassAlpha, wetLowPassCoefficient);

        if (monoBassEnabled)
        {
            monoBassLowPass[0] += monoBassCoefficient * (wetLeft - monoBassLowPass[0]);
            monoBassLowPass[1] += monoBassCoefficient * (wetRight - monoBassLowPass[1]);

            const auto monoLow = (monoBassLowPass[0] + monoBassLowPass[1]) * 0.5f;
            wetLeft = (wetLeft - monoBassLowPass[0]) + monoLow;
            wetRight = (wetRight - monoBassLowPass[1]) + monoLow;
        }

        const auto currentWidth = juce::jlimit(0.0f, 1.45f, width.getNextValue() * (0.65f + modeSettings.widthScale * 0.65f));
        if (bufferChannels > 1)
        {
            const auto wetMid = (wetLeft + wetRight) * 0.5f;
            const auto wetSide = (wetLeft - wetRight) * 0.5f * currentWidth;
            wetLeft = wetMid + wetSide;
            wetRight = wetMid - wetSide;
        }

        const auto duckNormalised = juce::jlimit(0.0f, 1.0f, (juce::Decibels::gainToDecibels(duckEnvelope, -90.0f) + 44.0f) / 44.0f);
        const auto duckDb = juce::jlimit(0.0f, 14.0f,
                                         ducking.getNextValue() * 14.0f * std::pow(duckNormalised, 1.25f));
        const auto duckGain = juce::Decibels::decibelsToGain(-duckDb);
        const auto currentMix = mix.getNextValue();
        const auto dryGain = 1.0f - currentMix * 0.08f;
        const auto wetOutputGain = modeSettings.wetTrim * colorSettings.wetGain * (0.82f + earlyAmount * 0.07f + lateAmount * 0.08f);

        auto* leftSamples = buffer.getWritePointer(0);
        leftSamples[sample] = sanitizeSample(dryLeft * dryGain + wetLeft * duckGain * currentMix * wetOutputGain);

        if (bufferChannels > 1)
        {
            auto* rightSamples = buffer.getWritePointer(1);
            rightSamples[sample] = sanitizeSample(dryRight * dryGain + wetRight * duckGain * currentMix * wetOutputGain);
        }

        const auto tankInputScale = inputToTank * (0.22f + size * 0.08f);
        const auto tankInputLeft = (preLeft * 0.62f + earlyLeft * 0.38f) * tankInputScale;
        const auto tankInputRight = (preRight * 0.62f + earlyRight * 0.38f) * tankInputScale;
        const auto diffusionBlend = juce::jlimit(0.40f, 0.96f, modeSettings.diffusion * (0.66f + diffLate * 0.34f));

        for (auto line = 0; line < tankLineCount; ++line)
        {
            const auto index = static_cast<size_t>(line);
            const auto nextA = tankOut[static_cast<size_t>((line + 1) % tankLineCount)];
            const auto nextB = tankOut[static_cast<size_t>((line + 3) % tankLineCount)];
            const auto nextC = tankOut[static_cast<size_t>((line + 5) % tankLineCount)];
            const auto householder = tankSum * 0.25f - tankOut[index];
            const auto crossMix = nextA * 0.32f - nextB * 0.20f + nextC * 0.12f;
            const auto matrixFeedback = (householder * (0.82f + diffLate * 0.10f) + crossMix * (0.10f + diffLate * 0.06f))
                                      * diffusionBlend
                                      * tankFeedback[index];
            const auto stereoInjection = ((line & 1) == 0 ? tankInputLeft : tankInputRight) * (((line + 2) & 4) == 0 ? 1.0f : -1.0f);
            const auto monoInjection = monoPre * 0.035f * tankInputScale * (((line + 1) & 2) == 0 ? 1.0f : -1.0f);
            auto writeSample = processFeedbackFilters(stereoInjection + monoInjection + matrixFeedback,
                                                       line,
                                                       feedbackHighPassAlpha,
                                                       feedbackLowPassCoefficient);

            writeSample = applyColor(writeSample, colorDrive * 0.65f, colorLoFi * 0.5f);
            tankBuffer.setSample(line, tankWritePosition, sanitizeSample(writeSample));
        }

        if (++predelayWritePosition >= predelayBufferSize)
            predelayWritePosition = 0;

        if (++tankWritePosition >= tankBufferSize)
            tankWritePosition = 0;

        modPhase += juce::MathConstants<float>::twoPi * modRate / static_cast<float>(sampleRate);
        if (modPhase >= juce::MathConstants<float>::twoPi)
            modPhase -= juce::MathConstants<float>::twoPi;
    }

    sanitizeBuffer(buffer);
}

float ReverbModule::getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept
{
    if (parameter == nullptr)
        return fallback;

    const auto value = parameter->load();
    return std::isfinite(value) ? value : fallback;
}

int ReverbModule::getChoiceIndex(std::atomic<float>* parameter, int fallback, int maxIndex) const noexcept
{
    return juce::jlimit(0, maxIndex, juce::roundToInt(getParameterValue(parameter, static_cast<float>(fallback))));
}

ReverbModule::Mode ReverbModule::getMode() const noexcept
{
    switch (getChoiceIndex(parameters.reverbMode, 1, 8))
    {
        case 0: return Mode::smallRoom;
        case 1: return Mode::mediumRoom;
        case 3: return Mode::concertHall;
        case 4: return Mode::plate;
        case 5: return Mode::chamber;
        case 6: return Mode::cathedral;
        case 7: return Mode::ambience;
        case 8: return Mode::reverse;
        default: return Mode::largeHall;
    }
}

ReverbModule::Color ReverbModule::getColor() const noexcept
{
    switch (getChoiceIndex(parameters.reverbColor, 0, 5))
    {
        case 1: return Color::bright80s;
        case 2: return Color::dark70s;
        case 3: return Color::loFi;
        case 4: return Color::tape;
        case 5: return Color::digitalVintage;
        default: return Color::modernClean;
    }
}

ReverbModule::ModeSettings ReverbModule::getModeSettings(Mode mode) const noexcept
{
    switch (mode)
    {
        case Mode::smallRoom: return { 0.55f, 0.55f, 1.35f, 0.55f, 0.54f, 1.08f, 0.0f, 0.72f, 0.84f, 0.82f };
        case Mode::mediumRoom: return { 0.82f, 0.86f, 1.12f, 0.86f, 0.72f, 1.05f, 0.0f, 0.92f, 0.86f, 1.00f };
        case Mode::concertHall: return { 1.32f, 1.36f, 0.78f, 1.15f, 0.88f, 1.00f, 0.02f, 1.08f, 0.86f, 1.13f };
        case Mode::plate: return { 0.78f, 0.92f, 0.82f, 1.24f, 0.92f, 1.28f, 0.0f, 1.14f, 0.90f, 1.10f };
        case Mode::chamber: return { 0.72f, 0.82f, 1.05f, 0.95f, 0.82f, 0.96f, 0.0f, 1.00f, 0.86f, 0.98f };
        case Mode::cathedral: return { 1.58f, 1.78f, 0.60f, 1.26f, 0.90f, 0.92f, 0.05f, 1.18f, 0.82f, 1.22f };
        case Mode::ambience: return { 0.42f, 0.38f, 1.45f, 0.32f, 0.48f, 1.08f, 0.0f, 0.55f, 0.80f, 0.90f };
        case Mode::reverse: return { 1.02f, 1.08f, 0.45f, 1.08f, 0.80f, 1.12f, 0.28f, 0.98f, 0.86f, 1.08f };
        case Mode::largeHall:
        default: return { 1.14f, 1.18f, 0.90f, 1.05f, 0.78f, 1.02f, 0.01f, 1.04f, 0.88f, 1.08f };
    }
}

ReverbModule::ColorSettings ReverbModule::getColorSettings(Color color) const noexcept
{
    switch (color)
    {
        case Color::bright80s: return { 1.45f, 0.025f, 0.0f, 1.12f, 1.08f, 1.08f };
        case Color::dark70s: return { 0.55f, 0.05f, 0.0f, 0.72f, 0.92f, 0.78f };
        case Color::loFi: return { 0.60f, 0.09f, 0.52f, 0.62f, 0.88f, 0.72f };
        case Color::tape: return { 0.78f, 0.14f, 0.18f, 0.80f, 0.96f, 0.84f };
        case Color::digitalVintage: return { 1.08f, 0.02f, 0.24f, 1.25f, 1.02f, 0.95f };
        case Color::modernClean:
        default: return { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    }
}

float ReverbModule::getPredelayMs() const noexcept
{
    if (getParameterValue(parameters.reverbTempoSyncPredelay, 0.0f) > 0.5f)
    {
        const auto quarterNoteMs = static_cast<float>(60000.0 / juce::jlimit(20.0, 300.0, hostBpm));
        return juce::jlimit(0.0f, 250.0f, quarterNoteMs * 0.25f);
    }

    return juce::jlimit(0.0f, 250.0f, getParameterValue(parameters.reverbPredelayMs, 14.0f));
}

float ReverbModule::readPredelaySample(int channel, float delaySamples) const noexcept
{
    const auto safeDelay = juce::jlimit(1.0f, static_cast<float>(predelayBufferSize - 3), std::isfinite(delaySamples) ? delaySamples : 1.0f);
    auto readPosition = static_cast<float>(predelayWritePosition) - safeDelay;

    while (readPosition < 0.0f)
        readPosition += static_cast<float>(predelayBufferSize);

    const auto index0 = juce::jlimit(0, predelayBufferSize - 1, static_cast<int>(readPosition));
    const auto index1 = (index0 + 1) % predelayBufferSize;
    const auto fraction = readPosition - static_cast<float>(index0);
    const auto* data = predelayBuffer.getReadPointer(juce::jlimit(0, internalChannels - 1, channel));

    return data[index0] + (data[index1] - data[index0]) * fraction;
}

float ReverbModule::readTankSample(int line, float delaySamples) const noexcept
{
    const auto safeDelay = juce::jlimit(1.0f, static_cast<float>(tankBufferSize - 3), std::isfinite(delaySamples) ? delaySamples : 1.0f);
    auto readPosition = static_cast<float>(tankWritePosition) - safeDelay;

    while (readPosition < 0.0f)
        readPosition += static_cast<float>(tankBufferSize);

    const auto index0 = juce::jlimit(0, tankBufferSize - 1, static_cast<int>(readPosition));
    const auto index1 = (index0 + 1) % tankBufferSize;
    const auto fraction = readPosition - static_cast<float>(index0);
    const auto* data = tankBuffer.getReadPointer(juce::jlimit(0, tankLineCount - 1, line));

    return data[index0] + (data[index1] - data[index0]) * fraction;
}

float ReverbModule::processFeedbackFilters(float input, int line, float highPassAlphaValue, float lowPassCoefficientValue) noexcept
{
    const auto index = static_cast<size_t>(juce::jlimit(0, tankLineCount - 1, line));
    const auto safeInput = sanitizeSample(input);
    const auto highPassed = highPassAlphaValue * (feedbackHighPassY1[index] + safeInput - feedbackHighPassX1[index]);
    feedbackHighPassX1[index] = safeInput;
    feedbackHighPassY1[index] = sanitizeSample(highPassed);
    feedbackLowPassY1[index] += lowPassCoefficientValue * (feedbackHighPassY1[index] - feedbackLowPassY1[index]);
    return sanitizeSample(feedbackLowPassY1[index]);
}

float ReverbModule::processWetFilters(float input, int channel, float highPassAlphaValue, float lowPassCoefficientValue) noexcept
{
    const auto index = static_cast<size_t>(juce::jlimit(0, internalChannels - 1, channel));
    const auto safeInput = sanitizeSample(input);
    const auto highPassed = highPassAlphaValue * (wetHighPassY1[index] + safeInput - wetHighPassX1[index]);
    wetHighPassX1[index] = safeInput;
    wetHighPassY1[index] = sanitizeSample(highPassed);
    wetLowPassY1[index] += lowPassCoefficientValue * (wetHighPassY1[index] - wetLowPassY1[index]);
    return sanitizeSample(wetLowPassY1[index]);
}

float ReverbModule::applyColor(float input, float drive, float loFi) const noexcept
{
    auto output = sanitizeSample(input);
    drive = juce::jlimit(0.0f, 1.0f, std::isfinite(drive) ? drive : 0.0f);
    loFi = juce::jlimit(0.0f, 1.0f, std::isfinite(loFi) ? loFi : 0.0f);

    if (drive > 0.0001f)
    {
        const auto driveGain = 1.0f + drive * 3.0f;
        const auto driven = std::tanh(output * driveGain) / std::tanh(driveGain);
        output = output * (1.0f - drive) + driven * drive;
    }

    if (loFi > 0.0001f)
    {
        const auto levels = juce::jmax(24.0f, 12288.0f * (1.0f - loFi) + 96.0f * loFi);
        const auto quantized = std::round(output * levels) / levels;
        output = output * (1.0f - loFi) + quantized * loFi;
    }

    return sanitizeSample(output);
}

float ReverbModule::sanitizeSample(float value) const noexcept
{
    if (! std::isfinite(value))
        return 0.0f;

    if (std::abs(value) > 3.0f)
        return std::tanh(value / 3.0f) * 3.0f;

    return value;
}

void ReverbModule::sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept
{
    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            if (! std::isfinite(samples[sample]))
                samples[sample] = 0.0f;
    }
}
