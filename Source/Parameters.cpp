#include "Parameters.h"

namespace
{
    juce::String trimToLength(juce::String text, int maximumStringLength)
    {
        return maximumStringLength > 0 ? text.substring(0, maximumStringLength) : text;
    }

    std::unique_ptr<juce::AudioParameterFloat> makeGainParameter(const juce::String& id,
                                                                 const juce::String& name)
    {
        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 },
            name,
            juce::NormalisableRange<float> { -24.0f, 24.0f, 0.01f },
            0.0f,
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

    // These three parameters affect audio in the current pass-through stage.
    params.push_back(makeGainParameter(inputGainDb, "Input Gain"));
    params.push_back(makeGainParameter(outputGainDb, "Output Gain"));
    params.push_back(makeBoolParameter(globalBypass, "Global Bypass", false));

    // Module switches and macro amounts are wired to APVTS now and reserved for the next DSP pass.
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

    return { params.begin(), params.end() };
}
