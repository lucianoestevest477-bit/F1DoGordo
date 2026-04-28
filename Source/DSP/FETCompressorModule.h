#pragma once

#include "AudioModule.h"
#include <array>
#include <atomic>
#include <cstdint>

class FETCompressorModule final : public AudioModule
{
public:
    struct ParameterPointers
    {
        std::atomic<float>* compEnabled = nullptr;
        std::atomic<float>* compInputDb = nullptr;
        std::atomic<float>* compThresholdDb = nullptr;
        std::atomic<float>* compOutputDb = nullptr;
        std::atomic<float>* compAttack = nullptr;
        std::atomic<float>* compRelease = nullptr;
        std::atomic<float>* compRatio = nullptr;
        std::atomic<float>* compMix = nullptr;
        std::atomic<float>* compSidechainHpHz = nullptr;
        std::atomic<float>* compRevision = nullptr;
        std::atomic<float>* compNoiseMode = nullptr;
    };

    void setParameters(ParameterPointers newParameters) noexcept;
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;
    float getGainReductionDb() const noexcept { return gainReductionDb.load(); }
    float getInputPeak() const noexcept { return inputPeak.load(); }
    float getOutputPeak() const noexcept { return outputPeak.load(); }

private:
    enum class Revision
    {
        clean,
        blue,
        black,
        hot
    };

    enum class NoiseMode
    {
        off,
        low,
        high
    };

    float getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept;
    int getChoiceIndex(std::atomic<float>* parameter, int fallback, int maxIndex) const noexcept;
    float getRatio() const noexcept;
    Revision getRevision() const noexcept;
    NoiseMode getNoiseMode() const noexcept;
    void updateSidechainFilter(float frequencyHz) noexcept;
    float processSidechainHighPass(float input, int channel) noexcept;
    float getNoiseSample(NoiseMode mode) noexcept;
    void storeBypassedMeterLevels(const juce::AudioBuffer<float>& buffer) noexcept;
    void sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept;

    ParameterPointers parameters;
    juce::AudioBuffer<float> dryBuffer;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> inputGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix;

    std::array<float, 2> sidechainX1 {};
    std::array<float, 2> sidechainY1 {};
    float sidechainAlpha = 0.0f;
    float cachedSidechainHpHz = -1.0f;
    float detectorEnvelope = 0.0f;
    float displayedGainReductionDb = 0.0f;
    std::atomic<float> gainReductionDb { 0.0f };
    std::atomic<float> inputPeak { 0.0f };
    std::atomic<float> outputPeak { 0.0f };
    std::uint32_t noiseState = 0x1234abcd;
    bool prepared = false;
};
