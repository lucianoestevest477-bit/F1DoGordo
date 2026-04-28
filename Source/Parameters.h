#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace Parameters
{
    inline constexpr auto inputGainDb = "inputGainDb";
    inline constexpr auto outputGainDb = "outputGainDb";
    inline constexpr auto globalBypass = "globalBypass";
    inline constexpr auto channelEnabled = "channelEnabled";
    inline constexpr auto compEnabled = "compEnabled";
    inline constexpr auto airEnabled = "airEnabled";
    inline constexpr auto delayEnabled = "delayEnabled";
    inline constexpr auto reverbEnabled = "reverbEnabled";
    inline constexpr auto channelMix = "channelMix";
    inline constexpr auto compMix = "compMix";
    inline constexpr auto airMix = "airMix";
    inline constexpr auto delaySend = "delaySend";
    inline constexpr auto reverbSend = "reverbSend";
    inline constexpr auto masterWidth = "masterWidth";
    inline constexpr auto channelInputTrimDb = "channelInputTrimDb";
    inline constexpr auto phaseInvert = "phaseInvert";
    inline constexpr auto highPassHz = "highPassHz";
    inline constexpr auto lowPassHz = "lowPassHz";
    inline constexpr auto lowGainDb = "lowGainDb";
    inline constexpr auto lowFreqHz = "lowFreqHz";
    inline constexpr auto lowMidGainDb = "lowMidGainDb";
    inline constexpr auto lowMidFreqHz = "lowMidFreqHz";
    inline constexpr auto highMidGainDb = "highMidGainDb";
    inline constexpr auto highMidFreqHz = "highMidFreqHz";
    inline constexpr auto highGainDb = "highGainDb";
    inline constexpr auto highFreqHz = "highFreqHz";
    inline constexpr auto drive = "drive";
    inline constexpr auto compInputDb = "compInputDb";
    inline constexpr auto compOutputDb = "compOutputDb";
    inline constexpr auto compAttack = "compAttack";
    inline constexpr auto compRelease = "compRelease";
    inline constexpr auto compRatio = "compRatio";
    inline constexpr auto compSidechainHpHz = "compSidechainHpHz";
    inline constexpr auto compRevision = "compRevision";
    inline constexpr auto compNoiseMode = "compNoiseMode";

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
