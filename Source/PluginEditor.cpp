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

    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::inputGainDb, dashboard.inputGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::outputGainDb, dashboard.outputGain));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::channelMix, dashboard.channelMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::compMix, dashboard.compMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::airMix, dashboard.airMix));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::delaySend, dashboard.delaySend));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::reverbSend, dashboard.reverbSend));
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(state, Parameters::masterWidth, dashboard.masterWidth));

    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::globalBypass, dashboard.globalBypass));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::channelEnabled, dashboard.channelEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::compEnabled, dashboard.compEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::airEnabled, dashboard.airEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::delayEnabled, dashboard.delayEnabled));
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(state, Parameters::reverbEnabled, dashboard.reverbEnabled));

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
    const auto input = juce::jlimit(0.0f, 1.0f, audioProcessor.getInputPeak());
    const auto output = juce::jlimit(0.0f, 1.0f, audioProcessor.getOutputPeak());
    const auto gr = juce::jlimit(0.0f, 1.0f, std::abs(audioProcessor.getGainReductionDb()) / 24.0f);

    dashboard.setMeterLevels(input, output, gr);
    topBar.setCpuText("CPU --");
}
