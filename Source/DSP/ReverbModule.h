#pragma once

#include "AudioModule.h"
#include <array>
#include <atomic>

class ReverbModule final : public AudioModule
{
public:
    struct ParameterPointers
    {
        std::atomic<float>* reverbEnabled = nullptr;
        std::atomic<float>* reverbMix = nullptr;
        std::atomic<float>* reverbPredelayMs = nullptr;
        std::atomic<float>* reverbDecaySec = nullptr;
        std::atomic<float>* reverbSize = nullptr;
        std::atomic<float>* reverbAttack = nullptr;
        std::atomic<float>* reverbEarly = nullptr;
        std::atomic<float>* reverbLate = nullptr;
        std::atomic<float>* reverbLowCutHz = nullptr;
        std::atomic<float>* reverbHighCutHz = nullptr;
        std::atomic<float>* reverbLowDampHz = nullptr;
        std::atomic<float>* reverbHighDampDb = nullptr;
        std::atomic<float>* reverbDiffusionEarly = nullptr;
        std::atomic<float>* reverbDiffusionLate = nullptr;
        std::atomic<float>* reverbModRate = nullptr;
        std::atomic<float>* reverbModDepth = nullptr;
        std::atomic<float>* reverbWidth = nullptr;
        std::atomic<float>* reverbMode = nullptr;
        std::atomic<float>* reverbColor = nullptr;
        std::atomic<float>* reverbFreeze = nullptr;
        std::atomic<float>* reverbDucking = nullptr;
        std::atomic<float>* reverbTempoSyncPredelay = nullptr;
        std::atomic<float>* reverbMonoBass = nullptr;
    };

    void setParameters(ParameterPointers newParameters) noexcept;
    void setHostBpm(double bpm) noexcept;
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;

private:
    enum class Mode
    {
        smallRoom,
        mediumRoom,
        largeHall,
        concertHall,
        plate,
        chamber,
        cathedral,
        ambience,
        reverse
    };

    enum class Color
    {
        modernClean,
        bright80s,
        dark70s,
        loFi,
        tape,
        digitalVintage
    };

    struct ModeSettings
    {
        float sizeScale = 1.0f;
        float decayScale = 1.0f;
        float earlyScale = 1.0f;
        float lateScale = 1.0f;
        float diffusion = 0.65f;
        float brightness = 1.0f;
        float attackBias = 0.0f;
        float tailLevel = 1.0f;
        float wetTrim = 0.82f;
        float widthScale = 1.0f;
    };

    struct ColorSettings
    {
        float brightness = 1.0f;
        float drive = 0.0f;
        float loFi = 0.0f;
        float modulation = 1.0f;
        float wetGain = 1.0f;
        float damping = 1.0f;
    };

    static constexpr auto internalChannels = 2;
    static constexpr auto tankLineCount = 8;

    float getParameterValue(std::atomic<float>* parameter, float fallback) const noexcept;
    int getChoiceIndex(std::atomic<float>* parameter, int fallback, int maxIndex) const noexcept;
    Mode getMode() const noexcept;
    Color getColor() const noexcept;
    ModeSettings getModeSettings(Mode mode) const noexcept;
    ColorSettings getColorSettings(Color color) const noexcept;
    float getPredelayMs() const noexcept;
    float readPredelaySample(int channel, float delaySamples) const noexcept;
    float readTankSample(int line, float delaySamples) const noexcept;
    float processFeedbackFilters(float input, int line, float highPassAlpha, float lowPassCoefficient) noexcept;
    float processWetFilters(float input, int channel, float highPassAlpha, float lowPassCoefficient) noexcept;
    float applyColor(float input, float drive, float loFi) const noexcept;
    float sanitizeSample(float value) const noexcept;
    void sanitizeBuffer(juce::AudioBuffer<float>& buffer) const noexcept;

    ParameterPointers parameters;

    juce::AudioBuffer<float> predelayBuffer;
    juce::AudioBuffer<float> tankBuffer;
    int predelayBufferSize = 0;
    int tankBufferSize = 0;
    int predelayWritePosition = 0;
    int tankWritePosition = 0;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> predelaySamples;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> width;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> ducking;

    std::array<float, tankLineCount> feedbackHighPassX1 {};
    std::array<float, tankLineCount> feedbackHighPassY1 {};
    std::array<float, tankLineCount> feedbackLowPassY1 {};
    std::array<float, internalChannels> wetHighPassX1 {};
    std::array<float, internalChannels> wetHighPassY1 {};
    std::array<float, internalChannels> wetLowPassY1 {};
    std::array<float, internalChannels> monoBassLowPass {};

    double hostBpm = 120.0;
    float modPhase = 0.0f;
    float duckEnvelope = 0.0f;
    float attackEnvelope = 0.0f;
    bool prepared = false;
};
