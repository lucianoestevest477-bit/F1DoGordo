#pragma once

#include "AudioModule.h"
#include <array>
#include <atomic>
#include <limits>

class DelayModule final : public AudioModule
{
public:
    struct ParameterPointers
    {
        std::atomic<float>* delayEnabled = nullptr;
        std::atomic<float>* delayTimeMs = nullptr;
        std::atomic<float>* delaySyncEnabled = nullptr;
        std::atomic<float>* delayNoteDivision = nullptr;
        std::atomic<float>* delayMode = nullptr;
        std::atomic<float>* delayFeedback = nullptr;
        std::atomic<float>* delayLeftTime = nullptr;
        std::atomic<float>* delayRightTime = nullptr;
        std::atomic<float>* delayLink = nullptr;
        std::atomic<float>* delayLoFi = nullptr;
        std::atomic<float>* delayModDepth = nullptr;
        std::atomic<float>* delayModRate = nullptr;
        std::atomic<float>* delayHighPassHz = nullptr;
        std::atomic<float>* delayLowPassHz = nullptr;
        std::atomic<float>* delayDucking = nullptr;
        std::atomic<float>* delayFreeze = nullptr;
        std::atomic<float>* delayStereoWidth = nullptr;
        std::atomic<float>* delaySend = nullptr;
    };

    void setParameters(ParameterPointers newParameters) noexcept;
    void setHostBpm(double bpm) noexcept;
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;

private:
    enum class Mode
    {
        mono,
        pingPong,
        dual
    };

    struct CachedFilters
    {
        float highPassHz = std::numeric_limits<float>::quiet_NaN();
        float lowPassHz = std::numeric_limits<float>::quiet_NaN();
        float loFi = std::numeric_limits<float>::quiet_NaN();
    };

    float getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept;
    int getChoiceIndex(std::atomic<float>* parameter, int fallback, int maxIndex) const noexcept;
    Mode getMode() const noexcept;
    float getSyncedDelayMs() const noexcept;
    void updateFilterCoefficients(bool force) noexcept;
    float readDelaySample(int channel, float delaySamples) const noexcept;
    float processWetFilters(float input, int channel) noexcept;
    float applyLoFi(float input, float amount) const noexcept;
    float sanitizeSample(float value) const noexcept;
    void sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept;

    ParameterPointers parameters;
    CachedFilters cachedFilters;

    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSize = 0;
    int writePosition = 0;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> leftDelaySamples;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> rightDelaySamples;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> feedback;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> send;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> stereoWidth;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> loFi;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modDepth;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> ducking;

    std::array<float, 2> highPassX1 {};
    std::array<float, 2> highPassY1 {};
    std::array<float, 2> lowPassY1 {};
    float highPassAlpha = 0.0f;
    float lowPassCoefficient = 1.0f;

    double hostBpm = 120.0;
    float duckEnvelope = 0.0f;
    float modPhase = 0.0f;
    bool prepared = false;
};
