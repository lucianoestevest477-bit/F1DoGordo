#pragma once

#include "AudioModule.h"
#include <atomic>
#include <limits>

class AirExciterModule final : public AudioModule
{
public:
    struct ParameterPointers
    {
        std::atomic<float>* airEnabled = nullptr;
        std::atomic<float>* airAmount = nullptr;
        std::atomic<float>* airMidAir = nullptr;
        std::atomic<float>* airHighAir = nullptr;
        std::atomic<float>* airFrequencyHz = nullptr;
        // airDrive is kept as the stable parameter ID; user-facing label is Air Intensity.
        std::atomic<float>* airDrive = nullptr;
        std::atomic<float>* airDensity = nullptr;
        std::atomic<float>* airDynamic = nullptr;
        std::atomic<float>* airDeEss = nullptr;
        std::atomic<float>* airTone = nullptr;
        std::atomic<float>* airOutputDb = nullptr;
        std::atomic<float>* airMix = nullptr;
    };

    void setParameters(ParameterPointers newParameters) noexcept;
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;

private:
    using IIRCoefficients = juce::dsp::IIR::Coefficients<float>;
    using IIRFilter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, IIRCoefficients>;

    struct CachedParameters
    {
        float frequencyHz = std::numeric_limits<float>::quiet_NaN();
        float midFrequencyHz = std::numeric_limits<float>::quiet_NaN();
        float sibilanceFrequencyHz = std::numeric_limits<float>::quiet_NaN();
        float tone = std::numeric_limits<float>::quiet_NaN();
    };

    void updateFilterCoefficients(bool force);
    float getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept;
    float sanitizeFrequency(float value) const noexcept;
    void sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept;

    ParameterPointers parameters;
    CachedParameters cached;

    IIRFilter midBandFilter;
    IIRFilter highPassFilter;
    IIRFilter toneShelfFilter;
    IIRFilter sibilanceFilter;

    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> midBuffer;
    juce::AudioBuffer<float> highBuffer;
    juce::AudioBuffer<float> sibilanceBuffer;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> amount;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> midAir;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> highAir;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> drive;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> density;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dynamic;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> deEss;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix;

    float dynamicEnvelope = 0.0f;
    float sibilanceEnvelope = 0.0f;
    bool prepared = false;
};
