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

    std::unique_ptr<juce::AudioParameterFloat> makeMillisecondsParameter(const juce::String& id,
                                                                         const juce::String& name,
                                                                         float minMs,
                                                                         float maxMs,
                                                                         float defaultValue,
                                                                         float centreMs)
    {
        juce::NormalisableRange<float> range { minMs, maxMs, 0.001f };
        range.setSkewForCentre(centreMs);

        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 },
            name,
            range,
            defaultValue,
            juce::AudioParameterFloatAttributes()
                .withLabel("ms")
                .withStringFromValueFunction([](float value, int maximumStringLength)
                {
                    const auto text = value < 1.0f
                                          ? juce::String(juce::roundToInt(value * 1000.0f)) + " us"
                                          : juce::String(value, value < 10.0f ? 2 : 0) + " ms";
                    return trimToLength(text, maximumStringLength);
                })
                .withValueFromStringFunction([](const juce::String& text)
                {
                    const auto parsed = text.retainCharacters("0123456789.-").getFloatValue();
                    return text.toLowerCase().contains("us") ? parsed / 1000.0f : parsed;
                }));
    }

    std::unique_ptr<juce::AudioParameterFloat> makeSecondsParameter(const juce::String& id,
                                                                    const juce::String& name,
                                                                    float minSeconds,
                                                                    float maxSeconds,
                                                                    float defaultValue,
                                                                    float centreSeconds)
    {
        juce::NormalisableRange<float> range { minSeconds, maxSeconds, 0.001f };
        range.setSkewForCentre(centreSeconds);

        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { id, 1 },
            name,
            range,
            defaultValue,
            juce::AudioParameterFloatAttributes()
                .withLabel("s")
                .withStringFromValueFunction([](float value, int maximumStringLength)
                {
                    return trimToLength(juce::String(value, value < 10.0f ? 2 : 1) + " s", maximumStringLength);
                })
                .withValueFromStringFunction([](const juce::String& text)
                {
                    return text.retainCharacters("0123456789.-").getFloatValue();
                }));
    }

    std::unique_ptr<juce::AudioParameterFloat> makeRateParameter(const juce::String& id,
                                                                 const juce::String& name,
                                                                 float minHz,
                                                                 float maxHz,
                                                                 float defaultValue,
                                                                 float centreHz)
    {
        juce::NormalisableRange<float> range { minHz, maxHz, 0.001f };
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
                    return trimToLength(juce::String(value, value < 1.0f ? 2 : 1) + " Hz", maximumStringLength);
                })
                .withValueFromStringFunction([](const juce::String& text)
                {
                    return text.retainCharacters("0123456789.-").getFloatValue();
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

    std::unique_ptr<juce::AudioParameterChoice> makeChoiceParameter(const juce::String& id,
                                                                    const juce::String& name,
                                                                    juce::StringArray choices,
                                                                    int defaultIndex)
    {
        return std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { id, 1 },
            name,
            choices,
            defaultIndex);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Factory default must be neutral: inserting the plugin should not change the sound.
    // These global parameters stay at unity/pass-through unless the user moves them.
    params.push_back(makeGainParameter(inputGainDb, "Input Gain"));
    params.push_back(makeGainParameter(outputGainDb, "Output Gain"));
    params.push_back(makeBoolParameter(globalBypass, "Global Bypass", false));

    // Module switches and macro amounts are wired to APVTS for UI/host automation.
    params.push_back(makeBoolParameter(channelEnabled, "Channel Enabled", false));
    params.push_back(makeBoolParameter(compEnabled, "FET Compressor Enabled", false));
    params.push_back(makeBoolParameter(airEnabled, "Air Exciter Enabled", false));
    params.push_back(makeBoolParameter(delayEnabled, "Delay Enabled", false));
    params.push_back(makeBoolParameter(reverbEnabled, "Reverb Enabled", false));
    params.push_back(makePercentParameter(channelMix, "Channel Mix", 0.0f));
    params.push_back(makePercentParameter(compMix, "Compressor Mix", 0.0f));
    params.push_back(makePercentParameter(airMix, "Air Mix", 0.0f));
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

    // FET compressor parameters: active DSP in FETCompressorModule.
    params.push_back(makeGainParameter(compInputDb, "Compressor Input", -24.0f, 24.0f));
    params.push_back(makeGainParameter(compThresholdDb, "Compressor Threshold", -60.0f, 0.0f, 0.0f));
    params.push_back(makeGainParameter(compOutputDb, "Compressor Output", -24.0f, 24.0f));
    params.push_back(makeMillisecondsParameter(compAttack, "Compressor Attack", 0.02f, 2.0f, 0.45f, 0.2f));
    params.push_back(makeMillisecondsParameter(compRelease, "Compressor Release", 50.0f, 1200.0f, 220.0f, 250.0f));
    params.push_back(makeChoiceParameter(compRatio, "Compressor Ratio", { "4:1", "8:1", "12:1", "20:1", "All" }, 0));
    params.push_back(makeFrequencyParameter(compSidechainHpHz, "Compressor Sidechain HPF", 20.0f, 500.0f, 90.0f, 120.0f));
    params.push_back(makeChoiceParameter(compRevision, "Compressor Revision", { "Clean", "Blue", "Black", "Hot" }, 0));
    params.push_back(makeChoiceParameter(compNoiseMode, "Compressor Noise", { "Off", "Low", "High" }, 0));

    // Air Exciter parameters: active DSP in AirExciterModule. Defaults keep the module neutral.
    params.push_back(makePercentParameter(airAmount, "Air Amount", 0.0f));
    params.push_back(makePercentParameter(airMidAir, "Mid Air", 0.0f));
    params.push_back(makePercentParameter(airHighAir, "High Air", 0.0f));
    params.push_back(makeFrequencyParameter(airFrequencyHz, "Air Frequency", 2000.0f, 16000.0f, 8000.0f, 7000.0f));
    // Keep the airDrive ID for preset/session compatibility; it controls Air intensity / harmonic excitation intensity.
    params.push_back(makePercentParameter(airDrive, "Air Intensity", 0.0f));
    params.push_back(makePercentParameter(airDensity, "Air Density", 0.0f));
    params.push_back(makePercentParameter(airDynamic, "Air Dynamic", 0.0f));
    params.push_back(makePercentParameter(airDeEss, "Air De-ess", 0.0f));
    params.push_back(makePercentParameter(airTone, "Air Tone", 0.5f));
    params.push_back(makeGainParameter(airOutputDb, "Air Output", -12.0f, 12.0f));

    // Delay parameters: active DSP in DelayModule. Amount is controlled by delaySend/ECHO only.
    params.push_back(makeMillisecondsParameter(delayTimeMs, "Delay Time", 20.0f, 2000.0f, 350.0f, 350.0f));
    params.push_back(makeBoolParameter(delaySyncEnabled, "Delay Sync", false));
    params.push_back(makeChoiceParameter(delayNoteDivision, "Delay Division", { "1/4", "1/8", "1/8D", "1/8T", "1/16", "1/16D", "1/16T", "1/2" }, 0));
    params.push_back(makeChoiceParameter(delayMode, "Delay Mode", { "Mono", "PingPong", "Dual" }, 0));
    params.push_back(makePercentParameter(delayFeedback, "Delay Feedback", 0.25f));
    params.push_back(makeMillisecondsParameter(delayLeftTime, "Delay Left Time", 20.0f, 2000.0f, 350.0f, 350.0f));
    params.push_back(makeMillisecondsParameter(delayRightTime, "Delay Right Time", 20.0f, 2000.0f, 525.0f, 525.0f));
    params.push_back(makeBoolParameter(delayLink, "Delay Link", true));
    params.push_back(makePercentParameter(delayLoFi, "Delay LoFi", 0.0f));
    params.push_back(makePercentParameter(delayModDepth, "Delay Mod Depth", 0.0f));
    params.push_back(makeRateParameter(delayModRate, "Delay Mod Rate", 0.05f, 8.0f, 0.35f, 0.7f));
    params.push_back(makeFrequencyParameter(delayHighPassHz, "Delay High-pass", 20.0f, 1000.0f, 20.0f, 120.0f));
    params.push_back(makeFrequencyParameter(delayLowPassHz, "Delay Low-pass", 1000.0f, 20000.0f, 16000.0f, 7000.0f));
    params.push_back(makePercentParameter(delayDucking, "Delay Ducking", 0.0f));
    params.push_back(makeBoolParameter(delayFreeze, "Delay Freeze", false));
    params.push_back(makePercentParameter(delayStereoWidth, "Delay Stereo Width", 1.0f));

    // Reverb parameters: active DSP in ReverbModule. reverbSend is kept as a legacy macro ID.
    params.push_back(makePercentParameter(reverbMix, "Reverb Mix", 0.0f));
    params.push_back(makeMillisecondsParameter(reverbPredelayMs, "Reverb Predelay", 0.0f, 250.0f, 20.0f, 45.0f));
    params.push_back(makeSecondsParameter(reverbDecaySec, "Reverb Decay", 0.3f, 14.0f, 2.4f, 2.5f));
    params.push_back(makePercentParameter(reverbSize, "Reverb Size", 0.55f));
    params.push_back(makePercentParameter(reverbAttack, "Reverb Attack", 0.0f));
    params.push_back(makePercentParameter(reverbEarly, "Reverb Early", 0.35f));
    params.push_back(makePercentParameter(reverbLate, "Reverb Late", 0.75f));
    params.push_back(makeFrequencyParameter(reverbLowCutHz, "Reverb Low Cut", 20.0f, 1000.0f, 120.0f, 140.0f));
    params.push_back(makeFrequencyParameter(reverbHighCutHz, "Reverb High Cut", 1000.0f, 20000.0f, 12000.0f, 7000.0f));
    params.push_back(makeFrequencyParameter(reverbLowDampHz, "Reverb Low Damp", 40.0f, 1000.0f, 220.0f, 220.0f));
    params.push_back(makeGainParameter(reverbHighDampDb, "Reverb High Damp", -24.0f, 0.0f, -6.0f));
    params.push_back(makePercentParameter(reverbDiffusionEarly, "Reverb Early Diffusion", 0.45f));
    params.push_back(makePercentParameter(reverbDiffusionLate, "Reverb Late Diffusion", 0.65f));
    params.push_back(makeRateParameter(reverbModRate, "Reverb Mod Rate", 0.05f, 5.0f, 0.25f, 0.6f));
    params.push_back(makePercentParameter(reverbModDepth, "Reverb Mod Depth", 0.10f));
    params.push_back(makePercentParameter(reverbWidth, "Reverb Width", 1.0f));
    params.push_back(makeChoiceParameter(reverbMode, "Reverb Mode", { "SmallRoom", "MediumRoom", "LargeHall", "ConcertHall", "Plate", "Chamber", "Cathedral", "Ambience", "Reverse" }, 2));
    params.push_back(makeChoiceParameter(reverbColor, "Reverb Color", { "ModernClean", "Bright80s", "Dark70s", "LoFi", "Tape", "DigitalVintage" }, 0));
    params.push_back(makeBoolParameter(reverbFreeze, "Reverb Freeze", false));
    params.push_back(makePercentParameter(reverbDucking, "Reverb Ducking", 0.0f));
    params.push_back(makeBoolParameter(reverbTempoSyncPredelay, "Reverb Sync Predelay", false));
    params.push_back(makeBoolParameter(reverbMonoBass, "Reverb Mono Bass", false));

    return { params.begin(), params.end() };
}
