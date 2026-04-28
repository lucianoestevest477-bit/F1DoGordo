#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/F1TopBar.h"
#include "UI/F1Dashboard.h"

class F1DoGordoAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                            private juce::Timer
{
public:
    explicit F1DoGordoAudioProcessorEditor(F1DoGordoAudioProcessor& processor);
    ~F1DoGordoAudioProcessorEditor() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void timerCallback() override;

    F1DoGordoAudioProcessor& audioProcessor;
    F1TopBar topBar;
    F1Dashboard dashboard;

    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(F1DoGordoAudioProcessorEditor)
};
