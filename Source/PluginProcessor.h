#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters.h"
#include "DSP/ChannelModule.h"
#include "DSP/FETCompressorModule.h"
#include "DSP/AirExciterModule.h"
#include "DSP/DelayModule.h"
#include "DSP/ReverbModule.h"
#include "DSP/Metering.h"

class F1DoGordoAudioProcessor final : public juce::AudioProcessor
{
public:
    F1DoGordoAudioProcessor();
    ~F1DoGordoAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    float getInputPeak() const noexcept { return inputMeter.getPeak(); }
    float getOutputPeak() const noexcept { return outputMeter.getPeak(); }
    float getCompressorInputPeak() const noexcept { return compressor.getInputPeak(); }
    float getCompressorOutputPeak() const noexcept { return compressor.getOutputPeak(); }
    float getGainReductionDb() const noexcept { return compressor.getGainReductionDb(); }

private:
    std::atomic<float>* inputGainDbParam = nullptr;
    std::atomic<float>* outputGainDbParam = nullptr;
    std::atomic<float>* globalBypassParam = nullptr;

    ChannelModule channel;
    FETCompressorModule compressor;
    AirExciterModule airExciter;
    DelayModule delay;
    ReverbModule reverb;

    Metering inputMeter;
    Metering outputMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(F1DoGordoAudioProcessor)
};
