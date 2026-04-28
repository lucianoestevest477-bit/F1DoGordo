#include "Parameters.h"

namespace
{
    juce::String trimToLength(juce::String text, int maximumStringLength)
    {
        return maximumStringLength > 0 ? text.substring(0, maximumStringLength) : text;
    }

    std::unique_ptr<juce::AudioParameterFloat> makeGainParameter(const juce::String& id,
                                                                 const juce::String& name,
                                                                 float minDb = -24.0f,
                                                                 float maxDb = 24.0f,
                                                                 float defaultValue = 0.0f)
    {
        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 },
            name,
            juce::NormalisableRange<float> { minDb, maxDb, 0.01f },
            defaultValue,
            juce::AudioParameterFloatAttributes()
                .withLabel("dB")
                .withStringFromValueFunction([](float value, int maximumStringLength)
                {
                    return trimToLength(juce::String(value, 1) + " dB", maximumStringLength);
                })
                .withValueFromStringFunction([](const juce::String& text)
                {
                    return text.retainCharacters("0123456789.-").getFloatValue();
                }));
    }

    std::unique_ptr<juce::AudioParameterFloat> makeFrequencyParameter(const juce::String& id,
                                                                      const juce::String& name,
                                                                      float minHz,
                                                                      float maxHz,
                                                                      float defaultValue,
                                                                      float centreHz)
    {
        juce::NormalisableRange<float> range { minHz, maxHz, 0.01f };
        range.setSkewForCentre(centreHz);

        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 },
            name,
            range,
            defaultValue,
            juce::AudioParameterFloatAttributes()
                .withLabel("Hz")
                .withStringFromValueFunction([](float value, int maximumStringLength)
                {
                    const auto text = value >= 1000.0f
                                          ? juce::String(value / 1000.0f, 1) + " kHz"
                                          : juce::String(juce::roundToInt(value)) + " Hz";
                    return trimToLength(text, maximumStringLength);
                })
                .withValueFromStringFunction([](const juce::String& text)
                {
                    const auto parsed = text.retainCharacters("0123456789.-").getFloatValue();
                    return text.toLowerCase().contains("k") ? parsed * 1000.0f : parsed;
                }));
    }

    std::unique_ptr<juce::AudioParameterFloat> makePercentParameter(const juce::String& id,
                                                                    const juce::String& name,
                                                                    float defaultValue)
    {
        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 },
            name,
            juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f },
            defaultValue,
            juce::AudioParameterFloatAttributes()
                .withLabel("%")
                .withStringFromValueFunction([](float value, int maximumStringLength)
                {
                    return trimToLength(juce::String(juce::roundToInt(value * 100.0f)) + "%", maximumStringLength);
                })
                .withValueFromStringFunction([](const juce::String& text)
                {
                    const auto parsed = text.retainCharacters("0123456789.-").getFloatValue();
                    return text.contains("%") || parsed > 1.0f ? parsed / 100.0f : parsed;
                }));
    }

    std::unique_ptr<juce::AudioParameterBool> makeBoolParameter(const juce::String& id,
                                                                const juce::String& name,
                                                                bool defaultValue)
    {
        return std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID { id, 1 },
            name,
            defaultValue);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // These global parameters affect audio in the current pass-through stage.
    params.push_back(makeGainParameter(inputGainDb, "Input Gain"));
    params.push_back(makeGainParameter(outputGainDb, "Output Gain"));
    params.push_back(makeBoolParameter(globalBypass, "Global Bypass", false));

    // Module switches and macro amounts are wired to APVTS for UI/host automation.
    params.push_back(makeBoolParameter(channelEnabled, "Channel Enabled", true));
    params.push_back(makeBoolParameter(compEnabled, "FET Compressor Enabled", true));
    params.push_back(makeBoolParameter(airEnabled, "Air Exciter Enabled", true));
    params.push_back(makeBoolParameter(delayEnabled, "Delay Enabled", true));
    params.push_back(makeBoolParameter(reverbEnabled, "Reverb Enabled", true));
    params.push_back(makePercentParameter(channelMix, "Channel Mix", 1.0f));
    params.push_back(makePercentParameter(compMix, "Compressor Mix", 1.0f));
    params.push_back(makePercentParameter(airMix, "Air Mix", 1.0f));
    params.push_back(makePercentParameter(delaySend, "Delay Send", 0.0f));
    params.push_back(makePercentParameter(reverbSend, "Reverb Send", 0.0f));
    params.push_back(makePercentParameter(masterWidth, "Master Width", 1.0f));

    // Channel/EQ parameters: active DSP in ChannelModule.
    params.push_back(makeGainParameter(channelInputTrimDb, "Channel Input Trim", -18.0f, 18.0f));
    params.push_back(makeBoolParameter(phaseInvert, "Phase Invert", false));
    params.push_back(makeFrequencyParameter(highPassHz, "High-pass", 20.0f, 1000.0f, 20.0f, 120.0f));
    params.push_back(makeFrequencyParameter(lowPassHz, "Low-pass", 2000.0f, 20000.0f, 20000.0f, 8000.0f));
    params.push_back(makeGainParameter(lowGainDb, "Low Gain", -12.0f, 12.0f));
    params.push_back(makeFrequencyParameter(lowFreqHz, "Low Frequency", 40.0f, 400.0f, 100.0f, 120.0f));
    params.push_back(makeGainParameter(lowMidGainDb, "Low-mid Gain", -12.0f, 12.0f));
    params.push_back(makeFrequencyParameter(lowMidFreqHz, "Low-mid Frequency", 150.0f, 2000.0f, 450.0f, 600.0f));
    params.push_back(makeGainParameter(highMidGainDb, "High-mid Gain", -12.0f, 12.0f));
    params.push_back(makeFrequencyParameter(highMidFreqHz, "High-mid Frequency", 1000.0f, 8000.0f, 3000.0f, 3000.0f));
    params.push_back(makeGainParameter(highGainDb, "High Gain", -12.0f, 12.0f));
    params.push_back(makeFrequencyParameter(highFreqHz, "High Frequency", 3000.0f, 16000.0f, 8000.0f, 8000.0f));
    params.push_back(makePercentParameter(drive, "Drive", 0.0f));

    return { params.begin(), params.end() };
}
