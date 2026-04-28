#include "PluginProcessor.h"
#include "PluginEditor.h"

F1DoGordoAudioProcessor::F1DoGordoAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", Parameters::createParameterLayout())
{
    inputGainDbParam = apvts.getRawParameterValue(Parameters::inputGainDb);
    outputGainDbParam = apvts.getRawParameterValue(Parameters::outputGainDb);
    globalBypassParam = apvts.getRawParameterValue(Parameters::globalBypass);

    channel.setParameters({
        apvts.getRawParameterValue(Parameters::channelEnabled),
        apvts.getRawParameterValue(Parameters::channelInputTrimDb),
        apvts.getRawParameterValue(Parameters::phaseInvert),
        apvts.getRawParameterValue(Parameters::highPassHz),
        apvts.getRawParameterValue(Parameters::lowPassHz),
        apvts.getRawParameterValue(Parameters::lowGainDb),
        apvts.getRawParameterValue(Parameters::lowFreqHz),
        apvts.getRawParameterValue(Parameters::lowMidGainDb),
        apvts.getRawParameterValue(Parameters::lowMidFreqHz),
        apvts.getRawParameterValue(Parameters::highMidGainDb),
        apvts.getRawParameterValue(Parameters::highMidFreqHz),
        apvts.getRawParameterValue(Parameters::highGainDb),
        apvts.getRawParameterValue(Parameters::highFreqHz),
        apvts.getRawParameterValue(Parameters::drive),
        apvts.getRawParameterValue(Parameters::channelMix)
    });

    compressor.setParameters({
        apvts.getRawParameterValue(Parameters::compEnabled),
        apvts.getRawParameterValue(Parameters::compInputDb),
        apvts.getRawParameterValue(Parameters::compThresholdDb),
        apvts.getRawParameterValue(Parameters::compOutputDb),
        apvts.getRawParameterValue(Parameters::compAttack),
        apvts.getRawParameterValue(Parameters::compRelease),
        apvts.getRawParameterValue(Parameters::compRatio),
        apvts.getRawParameterValue(Parameters::compMix),
        apvts.getRawParameterValue(Parameters::compSidechainHpHz),
        apvts.getRawParameterValue(Parameters::compRevision),
        apvts.getRawParameterValue(Parameters::compNoiseMode)
    });

    airExciter.setParameters({
        apvts.getRawParameterValue(Parameters::airEnabled),
        apvts.getRawParameterValue(Parameters::airAmount),
        apvts.getRawParameterValue(Parameters::airFrequencyHz),
        apvts.getRawParameterValue(Parameters::airDrive),
        apvts.getRawParameterValue(Parameters::airTone),
        apvts.getRawParameterValue(Parameters::airOutputDb),
        apvts.getRawParameterValue(Parameters::airMix)
    });
}

void F1DoGordoAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const juce::dsp::ProcessSpec spec {
        sampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(getTotalNumOutputChannels())
    };

    channel.prepare(spec);
    compressor.prepare(spec);
    airExciter.prepare(spec);
    delay.prepare(spec);
    reverb.prepare(spec);
    inputMeter.reset();
    outputMeter.reset();
}

void F1DoGordoAudioProcessor::releaseResources()
{
    channel.reset();
    compressor.reset();
    airExciter.reset();
    delay.reset();
    reverb.reset();
}

bool F1DoGordoAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOutput = layouts.getMainOutputChannelSet();
    const auto& mainInput = layouts.getMainInputChannelSet();

    if (mainOutput != juce::AudioChannelSet::mono() && mainOutput != juce::AudioChannelSet::stereo())
        return false;

    return mainInput == mainOutput;
}

void F1DoGordoAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto totalInputChannels = getTotalNumInputChannels();
    const auto totalOutputChannels = getTotalNumOutputChannels();

    for (auto channelIndex = totalInputChannels; channelIndex < totalOutputChannels; ++channelIndex)
        buffer.clear(channelIndex, 0, buffer.getNumSamples());

    inputMeter.update(buffer);

    const auto bypassed = globalBypassParam != nullptr && globalBypassParam->load() > 0.5f;

    if (! bypassed)
    {
        // Current audio-affecting controls: global gain/bypass, Channel/EQ, FET compressor and Air Exciter.
        // Delay and Reverb remain neutral placeholders until their DSP pass.
        const auto inputGain = juce::Decibels::decibelsToGain(inputGainDbParam != nullptr ? inputGainDbParam->load() : 0.0f);
        const auto outputGain = juce::Decibels::decibelsToGain(outputGainDbParam != nullptr ? outputGainDbParam->load() : 0.0f);

        buffer.applyGain(inputGain);
        channel.process(buffer);
        compressor.process(buffer);
        airExciter.process(buffer);
        delay.process(buffer);
        reverb.process(buffer);
        buffer.applyGain(outputGain);
    }

    outputMeter.update(buffer);
}

juce::AudioProcessorEditor* F1DoGordoAudioProcessor::createEditor()
{
    return new F1DoGordoAudioProcessorEditor(*this);
}

void F1DoGordoAudioProcessor::setCurrentProgram(int)
{
}

const juce::String F1DoGordoAudioProcessor::getProgramName(int)
{
    return {};
}

void F1DoGordoAudioProcessor::changeProgramName(int, const juce::String&)
{
}

void F1DoGordoAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        if (auto xml = state.createXml())
            copyXmlToBinary(*xml, destData);
    }
}

void F1DoGordoAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
    {
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new F1DoGordoAudioProcessor();
}
