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

    addAndMakeVisible(topBar);
    addAndMakeVisible(dashboard);

    auto& state = audioProcessor.apvts;

    // GLOBAL screen control map: INPUT/OUTPUT, CHANNEL/TONE, PUNCH and AIR affect audio now.
    // ECHO/SPACE/WIDTH remain APVTS-wired macro controls reserved for future DSP modules.
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::inputGainDb, dashboard.inputGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::outputGainDb, dashboard.outputGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::channelMix, dashboard.channelMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compMix, dashboard.compMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airMix, dashboard.airMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delaySend, dashboard.delaySend));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbSend, dashboard.reverbSend));
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
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airFrequencyHz, dashboard.airFrequency));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airDrive, dashboard.airPageDrive));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airTone, dashboard.airPageTone));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airMix, dashboard.airPageMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airOutputDb, dashboard.airPageOutput));

    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::globalBypass, dashboard.globalBypass));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::channelEnabled, dashboard.channelEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::compEnabled, dashboard.compEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::airEnabled, dashboard.airEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::delayEnabled, dashboard.delayEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::reverbEnabled, dashboard.reverbEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::phaseInvert, dashboard.channelPhaseInvert));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::compEnabled, dashboard.compPageEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::airEnabled, dashboard.airPageEnabled));

    startTimerHz(24);
}

void F1DoGordoAudioProcessorEditor::paint(juce::Graphics& g)
{
    F1Theme::paintCarbonBackground(g, getLocalBounds());
}

void F1DoGordoAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    topBar.setBounds(area.removeFromTop(74));
    dashboard.setBounds(area);
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
