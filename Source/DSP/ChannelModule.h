#pragma once

#include "AudioModule.h"
#include <atomic>
#include <limits>

class ChannelModule final : public AudioModule
{
public:
    struct ParameterPointers
    {
        std::atomic<float>* channelEnabled = nullptr;
        std::atomic<float>* channelInputTrimDb = nullptr;
        std::atomic<float>* phaseInvert = nullptr;
        std::atomic<float>* highPassHz = nullptr;
        std::atomic<float>* lowPassHz = nullptr;
        std::atomic<float>* lowGainDb = nullptr;
        std::atomic<float>* lowFreqHz = nullptr;
        std::atomic<float>* lowMidGainDb = nullptr;
        std::atomic<float>* lowMidFreqHz = nullptr;
        std::atomic<float>* highMidGainDb = nullptr;
        std::atomic<float>* highMidFreqHz = nullptr;
        std::atomic<float>* highGainDb = nullptr;
        std::atomic<float>* highFreqHz = nullptr;
        std::atomic<float>* drive = nullptr;
        std::atomic<float>* channelMix = nullptr;
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
        float highPassHz = std::numeric_limits<float>::quiet_NaN();
        float lowPassHz = std::numeric_limits<float>::quiet_NaN();
        float lowGainDb = std::numeric_limits<float>::quiet_NaN();
        float lowFreqHz = std::numeric_limits<float>::quiet_NaN();
        float lowMidGainDb = std::numeric_limits<float>::quiet_NaN();
        float lowMidFreqHz = std::numeric_limits<float>::quiet_NaN();
        float highMidGainDb = std::numeric_limits<float>::quiet_NaN();
        float highMidFreqHz = std::numeric_limits<float>::quiet_NaN();
        float highGainDb = std::numeric_limits<float>::quiet_NaN();
        float highFreqHz = std::numeric_limits<float>::quiet_NaN();
    };

    void updateFilterCoefficients(bool force);
    float getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept;
    float sanitizeFrequency(float value, float minHz, float maxHz) const noexcept;
    void applyDrive(juce::AudioBuffer<float>& buffer, float amount) const noexcept;
    void sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept;

    ParameterPointers parameters;
    CachedParameters cached;

    IIRFilter highPassFilter;
    IIRFilter lowPassFilter;
    IIRFilter lowShelfFilter;
    IIRFilter lowMidFilter;
    IIRFilter highMidFilter;
    IIRFilter highShelfFilter;

    juce::AudioBuffer<float> dryBuffer;
    bool prepared = false;
};
