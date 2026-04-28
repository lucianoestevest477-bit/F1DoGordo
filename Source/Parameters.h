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

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
