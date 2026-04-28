#include "Parameters.h"

namespace
{
    std::unique_ptr<juce::AudioParameterFloat> makeGainParameter(const juce::String& id,
                                                                 const juce::String& name)
    {
        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 },
            name,
            juce::NormalisableRange<float> { -24.0f, 24.0f, 0.01f },
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB"));
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
            juce::AudioParameterFloatAttributes().withLabel("%"));
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

    params.push_back(makeGainParameter(inputGainDb, "Input Gain"));
    params.push_back(makeGainParameter(outputGainDb, "Output Gain"));
    params.push_back(makeBoolParameter(globalBypass, "Global Bypass", false));
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
