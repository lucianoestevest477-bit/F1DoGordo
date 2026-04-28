#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include "GordoKnob.h"
#include "GordoButton.h"
#include "GordoLED.h"
#include "GordoMeter.h"

class F1Dashboard final : public juce::Component
{
public:
    F1Dashboard();

    void setMeterLevels(float input, float output, float gainReduction);
    void paint(juce::Graphics& g) override;
    void resized() override;

    GordoKnob inputGain { "INPUT" };
    GordoKnob outputGain { "OUTPUT" };
    GordoKnob channelMix { "CHANNEL" };
    GordoKnob compMix { "COMP" };
    GordoKnob airMix { "AIR" };
    GordoKnob delaySend { "DELAY" };
    GordoKnob reverbSend { "REVERB" };
    GordoKnob masterWidth { "WIDTH" };

    GordoButton globalBypass { "GLOBAL" };
    GordoButton channelEnabled { "CHANNEL" };
    GordoButton compEnabled { "COMP" };
    GordoButton airEnabled { "AIR" };
    GordoButton delayEnabled { "DELAY" };
    GordoButton reverbEnabled { "REVERB" };

private:
    GordoButton tabGlobal { "GLOBAL" };
    GordoButton tabChannel { "CHANNEL" };
    GordoButton tabComp { "COMP" };
    GordoButton tabAir { "AIR" };
    GordoButton tabDelay { "DELAY" };
    GordoButton tabReverb { "REVERB" };
    GordoButton tabRouting { "ROUTING" };
    GordoButton tabMeters { "METERS" };

    std::array<GordoButton*, 8> tabs {
        &tabGlobal,
        &tabChannel,
        &tabComp,
        &tabAir,
        &tabDelay,
        &tabReverb,
        &tabRouting,
        &tabMeters
    };

    std::array<GordoLED, 5> moduleLeds;
    GordoMeter inputMeter { "IN" };
    GordoMeter outputMeter { "OUT" };
    GordoMeter gainReductionMeter { "GR" };
};
