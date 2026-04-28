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
