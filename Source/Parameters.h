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
    inline constexpr auto compThresholdDb = "compThresholdDb";
    inline constexpr auto compOutputDb = "compOutputDb";
    inline constexpr auto compAttack = "compAttack";
    inline constexpr auto compRelease = "compRelease";
    inline constexpr auto compRatio = "compRatio";
    inline constexpr auto compSidechainHpHz = "compSidechainHpHz";
    inline constexpr auto compRevision = "compRevision";
    inline constexpr auto compNoiseMode = "compNoiseMode";
    inline constexpr auto airAmount = "airAmount";
    inline constexpr auto airMidAir = "airMidAir";
    inline constexpr auto airHighAir = "airHighAir";
    inline constexpr auto airFrequencyHz = "airFrequencyHz";
    inline constexpr auto airDrive = "airDrive";
    inline constexpr auto airDensity = "airDensity";
    inline constexpr auto airDynamic = "airDynamic";
    inline constexpr auto airDeEss = "airDeEss";
    inline constexpr auto airTone = "airTone";
    inline constexpr auto airOutputDb = "airOutputDb";
    inline constexpr auto delayTimeMs = "delayTimeMs";
    inline constexpr auto delaySyncEnabled = "delaySyncEnabled";
    inline constexpr auto delayNoteDivision = "delayNoteDivision";
    inline constexpr auto delayMode = "delayMode";
    inline constexpr auto delayFeedback = "delayFeedback";
    inline constexpr auto delayLeftTime = "delayLeftTime";
    inline constexpr auto delayRightTime = "delayRightTime";
    inline constexpr auto delayLink = "delayLink";
    inline constexpr auto delayLoFi = "delayLoFi";
    inline constexpr auto delayModDepth = "delayModDepth";
    inline constexpr auto delayModRate = "delayModRate";
    inline constexpr auto delayHighPassHz = "delayHighPassHz";
    inline constexpr auto delayLowPassHz = "delayLowPassHz";
    inline constexpr auto delayDucking = "delayDucking";
    inline constexpr auto delayFreeze = "delayFreeze";
    inline constexpr auto delayStereoWidth = "delayStereoWidth";

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
