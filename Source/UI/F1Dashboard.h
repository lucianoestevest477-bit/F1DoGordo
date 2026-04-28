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
    void setModuleStates(bool channelOn, bool compOn, bool airOn, bool delayOn, bool reverbOn);
    bool isCompressorPage() const noexcept;
    void paint(juce::Graphics& g) override;
    void resized() override;

    GordoKnob inputGain { "INPUT" };
    GordoKnob outputGain { "OUTPUT" };
    GordoKnob channelMix { "TONE" };
    GordoKnob compMix { "PUNCH" };
    GordoKnob airMix { "AIR" };
    GordoKnob delaySend { "ECHO" };
    GordoKnob reverbSend { "SPACE" };
    GordoKnob masterWidth { "WIDTH" };

    GordoButton globalBypass { "BYPASS" };
    GordoButton channelEnabled { "CHANNEL" };
    GordoButton compEnabled { "COMP" };
    GordoButton airEnabled { "AIR" };
    GordoButton delayEnabled { "DELAY" };
    GordoButton reverbEnabled { "REVERB" };

    GordoKnob channelInputTrim { "TRIM" };
    GordoKnob channelHighPass { "HPF" };
    GordoKnob channelLowPass { "LPF" };
    GordoKnob channelLowGain { "LOW" };
    GordoKnob channelLowFreq { "LOW FREQ" };
    GordoKnob channelLowMidGain { "LOW MID" };
    GordoKnob channelLowMidFreq { "LOW MID FREQ" };
    GordoKnob channelHighMidGain { "HIGH MID" };
    GordoKnob channelHighMidFreq { "HIGH MID FREQ" };
    GordoKnob channelHighGain { "HIGH" };
    GordoKnob channelHighFreq { "HIGH FREQ" };
    GordoKnob channelDrive { "DRIVE" };
    GordoKnob channelPageMix { "MIX" };
    GordoButton channelPhaseInvert { "PHASE" };

    GordoKnob compInput { "INPUT" };
    GordoKnob compThreshold { "THRESHOLD" };
    GordoKnob compOutput { "OUTPUT" };
    GordoKnob compAttack { "ATTACK" };
    GordoKnob compRelease { "RELEASE" };
    GordoKnob compRatio { "RATIO" };
    GordoKnob compPageMix { "MIX" };
    GordoKnob compSidechainHp { "SC HPF" };
    GordoKnob compRevision { "REVISION" };
    GordoKnob compNoise { "NOISE" };
    GordoButton compPageEnabled { "ON" };

    GordoKnob airPageAmount { "AMOUNT" };
    GordoKnob airFrequency { "FREQ" };
    GordoKnob airPageDrive { "DRIVE" };
    GordoKnob airPageTone { "TONE" };
    GordoKnob airPageMix { "MIX" };
    GordoKnob airPageOutput { "OUTPUT" };
    GordoButton airPageEnabled { "ON" };

private:
    enum class Page
    {
        global,
        channel,
        comp,
        air
    };

    void setPage(Page newPage);
    void updateControlVisibility();
    void layoutGlobalPage(juce::Rectangle<int> cockpit);
    void layoutChannelPage(juce::Rectangle<int> cockpit);
    void layoutCompPage(juce::Rectangle<int> cockpit);
    void layoutAirPage(juce::Rectangle<int> cockpit);

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
    GordoMeter gainReductionMeter { "GR dB" };
    Page activePage = Page::global;
};
