#include "PluginEditor.h"
#include "Parameters.h"
#include "UI/F1Theme.h"

F1DoGordoAudioProcessorEditor::F1DoGordoAudioProcessorEditor(F1DoGordoAudioProcessor& processor)
    : AudioProcessorEditor(&processor),
      audioProcessor(processor)
{
    setResizable(true, true);
    setResizeLimits(1200, 700, 2200, 1400);
    setSize(1200, 700);

    addAndMakeVisible(dashboard);
    addAndMakeVisible(topBar);

    auto& state = audioProcessor.apvts;

    // These controls are designed to be host-automation/controller-mapping friendly.
    // GLOBAL screen control map: INPUT/OUTPUT, CHANNEL/TONE, PUNCH, AIR, ECHO and SPACE affect audio now.
    // reverbSend remains in the APVTS as a legacy macro ID; SPACE controls the active reverbMix parameter.
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::inputGainDb, dashboard.inputGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::outputGainDb, dashboard.outputGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::channelMix, dashboard.channelMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compMix, dashboard.compMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airMix, dashboard.airMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delaySend, dashboard.delaySend));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbMix, dashboard.reverbSend));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::masterWidth, dashboard.masterWidth));

    // CHANNEL page control map: these parameters are processed by ChannelModule.
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::channelInputTrimDb, dashboard.channelInputTrim));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::highPassHz, dashboard.channelHighPass));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::lowPassHz, dashboard.channelLowPass));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::lowGainDb, dashboard.channelLowGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::lowFreqHz, dashboard.channelLowFreq));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::lowMidGainDb, dashboard.channelLowMidGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::lowMidFreqHz, dashboard.channelLowMidFreq));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::highMidGainDb, dashboard.channelHighMidGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::highMidFreqHz, dashboard.channelHighMidFreq));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::highGainDb, dashboard.channelHighGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::highFreqHz, dashboard.channelHighFreq));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::drive, dashboard.channelDrive));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::channelMix, dashboard.channelPageMix));

    // COMP page control map: these parameters are processed by FETCompressorModule.
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compInputDb, dashboard.compInput));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compThresholdDb, dashboard.compThreshold));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compOutputDb, dashboard.compOutput));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compAttack, dashboard.compAttack));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compRelease, dashboard.compRelease));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compRatio, dashboard.compRatio));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compMix, dashboard.compPageMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compSidechainHpHz, dashboard.compSidechainHp));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compRevision, dashboard.compRevision));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compNoiseMode, dashboard.compNoise));

    // AIR page control map: these parameters are processed by AirExciterModule.
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airAmount, dashboard.airPageAmount));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airMidAir, dashboard.airMidAir));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airHighAir, dashboard.airHighAir));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airFrequencyHz, dashboard.airFrequency));
    // airDrive is the stable APVTS ID for the user-facing INTENSITY control.
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airDrive, dashboard.airPageDrive));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airDensity, dashboard.airDensity));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airDynamic, dashboard.airDynamic));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airDeEss, dashboard.airDeEss));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airTone, dashboard.airPageTone));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airMix, dashboard.airPageMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airOutputDb, dashboard.airPageOutput));

    // DELAY page control map: delaySend/ECHO is the only delay amount control.
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayTimeMs, dashboard.delayTime));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayNoteDivision, dashboard.delayDivision));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayMode, dashboard.delayMode));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayFeedback, dashboard.delayFeedback));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delaySend, dashboard.delayPageSend));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayLeftTime, dashboard.delayLeftTime));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayRightTime, dashboard.delayRightTime));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayHighPassHz, dashboard.delayHighPass));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayLowPassHz, dashboard.delayLowPass));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayStereoWidth, dashboard.delayWidth));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayLoFi, dashboard.delayLoFi));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayModDepth, dashboard.delayModDepth));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayModRate, dashboard.delayModRate));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delayDucking, dashboard.delayDucking));

    // REVERB page control map: these parameters are processed by ReverbModule.
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbMix, dashboard.reverbPageMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbPredelayMs, dashboard.reverbPredelay));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbDecaySec, dashboard.reverbDecay));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbSize, dashboard.reverbSize));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbAttack, dashboard.reverbAttack));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbWidth, dashboard.reverbWidth));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbEarly, dashboard.reverbEarly));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbLate, dashboard.reverbLate));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbDiffusionEarly, dashboard.reverbDiffEarly));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbDiffusionLate, dashboard.reverbDiffLate));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbModRate, dashboard.reverbModRate));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbModDepth, dashboard.reverbModDepth));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbLowCutHz, dashboard.reverbLowCut));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbHighCutHz, dashboard.reverbHighCut));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbLowDampHz, dashboard.reverbLowDamp));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbHighDampDb, dashboard.reverbHighDamp));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbMode, dashboard.reverbMode));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbColor, dashboard.reverbColor));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbDucking, dashboard.reverbDucking));

    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::globalBypass, dashboard.globalBypass));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::channelEnabled, dashboard.channelEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::compEnabled, dashboard.compEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::airEnabled, dashboard.airEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::delayEnabled, dashboard.delayEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::reverbEnabled, dashboard.reverbEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::phaseInvert, dashboard.channelPhaseInvert));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::compEnabled, dashboard.compPageEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::airEnabled, dashboard.airPageEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::delaySyncEnabled, dashboard.delaySync));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::delayLink, dashboard.delayLink));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::delayFreeze, dashboard.delayFreeze));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::delayEnabled, dashboard.delayPageEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::reverbFreeze, dashboard.reverbFreeze));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::reverbTempoSyncPredelay, dashboard.reverbSyncPredelay));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::reverbMonoBass, dashboard.reverbMonoBass));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::reverbEnabled, dashboard.reverbPageEnabled));

    startTimerHz(24);
}

void F1DoGordoAudioProcessorEditor::paint(juce::Graphics& g)
{
    F1Theme::paintCarbonBackground(g, getLocalBounds());
}

void F1DoGordoAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    dashboard.setBounds(area);
    topBar.setBounds(area.removeFromTop(84).reduced(18, 6));
    topBar.toFront(false);
}

void F1DoGordoAudioProcessorEditor::timerCallback()
{
    const auto useCompressorMeters = dashboard.isCompressorPage();
    const auto input = juce::jlimit(0.0f, 1.0f, useCompressorMeters ? audioProcessor.getCompressorInputPeak()
                                                                     : audioProcessor.getInputPeak());
    const auto output = juce::jlimit(0.0f, 1.0f, useCompressorMeters ? audioProcessor.getCompressorOutputPeak()
                                                                      : audioProcessor.getOutputPeak());
    const auto gr = juce::jlimit(0.0f, 1.0f, audioProcessor.getGainReductionDb() / 24.0f);

    dashboard.setMeterLevels(input, output, gr);
    dashboard.setModuleStates(audioProcessor.apvts.getRawParameterValue(Parameters::channelEnabled)->load() > 0.5f,
                              audioProcessor.apvts.getRawParameterValue(Parameters::compEnabled)->load() > 0.5f,
                              audioProcessor.apvts.getRawParameterValue(Parameters::airEnabled)->load() > 0.5f,
                              audioProcessor.apvts.getRawParameterValue(Parameters::delayEnabled)->load() > 0.5f,
                              audioProcessor.apvts.getRawParameterValue(Parameters::reverbEnabled)->load() > 0.5f);
    topBar.setCpuText("CPU --");
}
