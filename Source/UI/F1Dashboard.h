#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cstdint>
#include "GordoKnob.h"
#include "GordoButton.h"
#include "GordoLED.h"
#include "GordoMeter.h"

class F1Dashboard final : public juce::Component
{
public:
    F1Dashboard();

    void setParameterState(juce::AudioProcessorValueTreeState& state) noexcept;
    void setMeterLevels(float input, float output, float gainReduction);
    void setModuleStates(bool channelOn, bool compOn, bool airOn, bool delayOn, bool reverbOn);
    bool isCompressorPage() const noexcept;
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

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
    GordoKnob airMidAir { "MID AIR" };
    GordoKnob airHighAir { "HIGH AIR" };
    GordoKnob airFrequency { "FREQ" };
    GordoKnob airPageDrive { "INTENSITY" };
    GordoKnob airDensity { "DENSITY" };
    GordoKnob airDynamic { "DYNAMIC" };
    GordoKnob airDeEss { "DE-ESS" };
    GordoKnob airPageTone { "TONE" };
    GordoKnob airPageMix { "MIX" };
    GordoKnob airPageOutput { "OUTPUT" };
    GordoButton airPageEnabled { "ON" };

    GordoKnob delayTime { "TIME" };
    GordoKnob delayDivision { "DIVISION" };
    GordoKnob delayMode { "MODE" };
    GordoKnob delayFeedback { "FEEDBACK" };
    GordoKnob delayPageSend { "SEND" };
    GordoKnob delayLeftTime { "L TIME" };
    GordoKnob delayRightTime { "R TIME" };
    GordoKnob delayHighPass { "HPF" };
    GordoKnob delayLowPass { "LPF" };
    GordoKnob delayWidth { "WIDTH" };
    GordoKnob delayLoFi { "LOFI" };
    GordoKnob delayModDepth { "MOD DEPTH" };
    GordoKnob delayModRate { "MOD RATE" };
    GordoKnob delayDucking { "DUCKING" };
    GordoButton delaySync { "SYNC" };
    GordoButton delayLink { "LINK" };
    GordoButton delayFreeze { "FREEZE" };
    GordoButton delayPageEnabled { "ON" };

    GordoKnob reverbPageMix { "MIX" };
    GordoKnob reverbPredelay { "PREDELAY" };
    GordoKnob reverbDecay { "DECAY" };
    GordoKnob reverbSize { "SIZE" };
    GordoKnob reverbAttack { "ATTACK" };
    GordoKnob reverbWidth { "WIDTH" };
    GordoKnob reverbEarly { "EARLY" };
    GordoKnob reverbLate { "LATE" };
    GordoKnob reverbDiffEarly { "DIFF EARLY" };
    GordoKnob reverbDiffLate { "DIFF LATE" };
    GordoKnob reverbModRate { "MOD RATE" };
    GordoKnob reverbModDepth { "MOD DEPTH" };
    GordoKnob reverbLowCut { "LOW CUT" };
    GordoKnob reverbHighCut { "HIGH CUT" };
    GordoKnob reverbLowDamp { "LOW DAMP" };
    GordoKnob reverbHighDamp { "HIGH DAMP" };
    GordoKnob reverbMode { "MODE" };
    GordoKnob reverbColor { "COLOR" };
    GordoKnob reverbDucking { "DUCKING" };
    GordoButton reverbFreeze { "FREEZE" };
    GordoButton reverbSyncPredelay { "SYNC PRE" };
    GordoButton reverbMonoBass { "MONO BASS" };
    GordoButton reverbPageEnabled { "ON" };

private:
    enum class Page
    {
        global,
        channel,
        comp,
        air,
        delay,
        routing,
        meters,
        reverb
    };

    enum class Hotspot
    {
        none,
        global,
        channel,
        comp,
        air,
        delay,
        routing,
        meters
    };

    enum class WheelControl
    {
        none,
        input,
        output,
        threshold,
        ratio,
        release,
        attack,
        gain,
        midAir,
        highAir,
        feedback,
        timing,
        bypass,
        compEnabled,
        airEnabled,
        delayEnabled,
        ab,
        save
    };

    enum class WheelControlKind
    {
        continuous,
        stepped,
        timing,
        toggle,
        placeholder
    };

    struct WheelControlZone
    {
        WheelControl control;
        WheelControlKind kind;
        const char* label;
        const char* parameterId;
        float x;
        float y;
        float width;
        float height;
        bool roundHitZone;
    };

    struct WheelPointer
    {
        WheelControl control;
        const char* id;
        const char* parameterId;
        juce::Image image;
        juce::Point<float> centerNormalized;
        juce::Rectangle<float> drawBoundsNormalized;
        float minAngleDegrees;
        float maxAngleDegrees;
        bool isDiscrete;
    };

    struct HotspotSpec
    {
        Hotspot hotspot;
        Page page;
        const char* label;
        float x;
        float y;
        float width;
        float height;
    };

    // Legacy module zones are kept only as a fallback/navigation map; the final UI uses wheelControlMap.
    static constexpr std::array<HotspotSpec, 7> hotspotMap {{
        { Hotspot::global,  Page::global,  "GLOBAL",  0.34f, 0.05f, 0.32f, 0.16f },
        { Hotspot::channel, Page::channel, "CHANNEL", 0.22f, 0.20f, 0.24f, 0.25f },
        { Hotspot::comp,    Page::comp,    "COMP",    0.54f, 0.20f, 0.24f, 0.25f },
        { Hotspot::air,     Page::air,     "AIR",     0.20f, 0.48f, 0.25f, 0.27f },
        { Hotspot::delay,   Page::delay,   "DELAY",   0.54f, 0.48f, 0.25f, 0.29f },
        { Hotspot::routing, Page::routing, "ROUTING", 0.39f, 0.70f, 0.22f, 0.22f },
        { Hotspot::meters,  Page::meters,  "METERS",  0.77f, 0.30f, 0.17f, 0.48f }
    }};

    static constexpr bool debugWheelZones = false;
    static constexpr bool debugWheelPointers = false;
    static constexpr bool debugWheelFunctionalMapping = true;

    // Final wheel control map. Coordinates are normalized to Assets/sss33VOLANTE.png.
    static constexpr std::array<WheelControlZone, 17> wheelControlMap {{
        { WheelControl::input,     WheelControlKind::continuous,  "INPUT",     "inputGainDb",       0.205f, 0.048f, 0.060f, 0.086f, true  },
        { WheelControl::output,    WheelControlKind::continuous,  "OUTPUT",    "outputGainDb",      0.733f, 0.048f, 0.060f, 0.086f, true  },
        { WheelControl::threshold, WheelControlKind::continuous,  "THRESHOLD", "compThresholdDb",   0.351f, 0.404f, 0.062f, 0.130f, true  },
        { WheelControl::ratio,     WheelControlKind::stepped,     "RATIO",     "compRatio",         0.404f, 0.188f, 0.078f, 0.150f, true  },
        { WheelControl::release,   WheelControlKind::continuous,  "RELEASE",   "compRelease",       0.523f, 0.193f, 0.078f, 0.150f, true  },
        { WheelControl::attack,    WheelControlKind::continuous,  "ATTACK",    "compAttack",        0.582f, 0.395f, 0.070f, 0.150f, true  },
        { WheelControl::gain,      WheelControlKind::continuous,  "GAIN",      "compOutputDb",      0.468f, 0.617f, 0.064f, 0.132f, true  },
        { WheelControl::midAir,    WheelControlKind::continuous,  "MID AIR",   "airMidAir",         0.268f, 0.538f, 0.050f, 0.096f, true  },
        { WheelControl::highAir,   WheelControlKind::continuous,  "HIGH AIR",  "airHighAir",        0.268f, 0.638f, 0.050f, 0.096f, true  },
        { WheelControl::feedback,  WheelControlKind::continuous,  "FEEDBACK",  "delayFeedback",     0.691f, 0.516f, 0.038f, 0.078f, true  },
        { WheelControl::timing,    WheelControlKind::timing,      "TIMING",    "delayNoteDivision", 0.691f, 0.651f, 0.038f, 0.078f, true  },
        { WheelControl::bypass,    WheelControlKind::toggle,      "BYPASS",    "globalBypass",      0.392f, 0.805f, 0.060f, 0.070f, false },
        { WheelControl::compEnabled,  WheelControlKind::toggle,   "COMP",      "compEnabled",       0.642f, 0.300f, 0.040f, 0.065f, true  },
        { WheelControl::airEnabled,   WheelControlKind::toggle,   "AIR",       "airEnabled",        0.270f, 0.455f, 0.052f, 0.075f, true  },
        { WheelControl::delayEnabled, WheelControlKind::toggle,   "DELAY",     "delayEnabled",      0.675f, 0.725f, 0.052f, 0.075f, true  },
        { WheelControl::ab,        WheelControlKind::placeholder, "A/B",       "",                  0.474f, 0.805f, 0.054f, 0.070f, false },
        { WheelControl::save,      WheelControlKind::placeholder, "SAVE",      "",                  0.548f, 0.805f, 0.060f, 0.070f, false }
    }};

    void setPage(Page newPage);
    void updateControlVisibility();
    void layoutGlobalPage(juce::Rectangle<int> cockpit);
    void layoutChannelPage(juce::Rectangle<int> cockpit);
    void layoutCompPage(juce::Rectangle<int> cockpit);
    void layoutAirPage(juce::Rectangle<int> cockpit);
    void layoutDelayPage(juce::Rectangle<int> cockpit);
    void layoutReverbPage(juce::Rectangle<int> cockpit);
    juce::Rectangle<float> getWheelImageBounds() const;
    juce::Rectangle<float> getHotspotBounds(Hotspot hotspot) const;
    juce::Rectangle<float> getWheelControlBounds(WheelControl control) const;
    bool isPointInsideWheelControl(WheelControl control, juce::Point<float> point) const;
    juce::Rectangle<float> getControlPanelBounds() const;
    Hotspot findHotspot(juce::Point<float> point) const;
    WheelControl findWheelControl(juce::Point<float> point) const;
    Page pageForHotspot(Hotspot hotspot) const;
    Hotspot hotspotForPage(Page page) const;
    juce::String getHotspotLabel(Hotspot hotspot) const;
    juce::Colour getHotspotColour(Hotspot hotspot) const;
    juce::String getWheelControlLabel(WheelControl control) const;
    const WheelControlZone* getWheelControlZone(WheelControl control) const;
    juce::Colour getWheelControlColour(WheelControl control) const;
    WheelControlKind getWheelControlKind(WheelControl control) const;
    GordoKnob* getKnobForWheelControl(WheelControl control) noexcept;
    GordoButton* getButtonForWheelControl(WheelControl control) noexcept;
    juce::RangedAudioParameter* getParameterById(const juce::String& parameterId) const;
    float getParameterNormalizedById(const juce::String& parameterId) const;
    float getParameterRawValueById(const juce::String& parameterId) const;
    juce::RangedAudioParameter* getWheelParameter(WheelControl control) const;
    juce::String getWheelParameterId(WheelControl control) const;
    float getWheelParameterNormalized(WheelControl control) const;
    int getWheelChoiceIndex(WheelControl control) const;
    int getWheelChoiceCount(WheelControl control) const;
    void beginWheelParameterGesture(WheelControl control);
    void setWheelParameterNormalized(WheelControl control, float normalizedValue);
    void setWheelChoiceIndex(WheelControl control, int index);
    void endWheelParameterGesture();
    void logWheelFunctionalMapping(WheelControl control, float oldNormalized, float newNormalized);
    void beginWheelControlDrag(WheelControl control, juce::Point<float> position);
    void dragWheelControl(juce::Point<float> position);
    void endWheelControlDrag();
    void incrementWheelControl(WheelControl control, int direction);
    int getTimingStepIndex(double value) const;
    void setTimingStepIndex(int index);
    void cycleTiming(int direction);
    void toggleWheelControl(WheelControl control);
    void makeModuleAudibleIfNeeded(WheelControl control);
    void makeOwningModuleAudibleForControl(WheelControl control);
    void ensureAirAudibleForWheelTest(WheelControl control, bool primeControlledValue);
    void setParameterNormalizedById(const juce::String& parameterId,
                                    float normalizedValue,
                                    const juce::String& labelForDebug);
    void showWheelControlTooltip(WheelControl control);
    void showAudibleStatusTooltip(WheelControl control, const juce::String& text);
    juce::String getWheelControlValueText(WheelControl control) const;
    juce::String getAirWheelDiagnosticText(WheelControl control) const;
    juce::String getModuleAudibleStateText(WheelControl control) const;
    void loadWheelPointerImages();
    void paintWheelPointers(juce::Graphics& g, juce::Rectangle<float> imageBounds);
    float getWheelPointerAngleDegrees(const WheelPointer& pointer);

    GordoButton tabGlobal { "GLOBAL" };
    GordoButton tabChannel { "CHANNEL" };
    GordoButton tabComp { "COMP" };
    GordoButton tabAir { "AIR" };
    GordoButton tabDelay { "DELAY" };
    GordoButton tabRouting { "ROUTING" };
    GordoButton tabMeters { "METERS" };

    std::array<GordoButton*, 7> tabs {
        &tabGlobal,
        &tabChannel,
        &tabComp,
        &tabAir,
        &tabDelay,
        &tabRouting,
        &tabMeters
    };

    juce::Image wheelImage;
    std::array<WheelPointer, 11> wheelPointers;
    juce::AudioProcessorValueTreeState* parameterState = nullptr;
    juce::RangedAudioParameter* activeWheelParameter = nullptr;
    Hotspot hoveredHotspot = Hotspot::none;
    WheelControl hoveredWheelControl = WheelControl::none;
    WheelControl activeWheelControl = WheelControl::none;
    juce::Point<float> dragStartPosition;
    float dragStartNormalized = 0.0f;
    int dragStartChoiceIndex = 0;
    uint32_t tooltipUntilMs = 0;
    uint32_t audibleStatusTooltipUntilMs = 0;
    WheelControl audibleStatusControl = WheelControl::none;
    juce::String audibleStatusTooltip;
    uint32_t debugOverlayUntilMs = 0;
    juce::String debugOverlayText;
    float latestGainReductionDb = 0.0f;

    std::array<GordoLED, 4> moduleLeds;
    GordoMeter inputMeter { "IN" };
    GordoMeter outputMeter { "OUT" };
    GordoMeter gainReductionMeter { "GR dB" };
    Page activePage = Page::global;
};
