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
        apvts.getRawParameterValue(Parameters::airMidAir),
        apvts.getRawParameterValue(Parameters::airHighAir),
        apvts.getRawParameterValue(Parameters::airFrequencyHz),
        apvts.getRawParameterValue(Parameters::airDrive),
        apvts.getRawParameterValue(Parameters::airDensity),
        apvts.getRawParameterValue(Parameters::airDynamic),
        apvts.getRawParameterValue(Parameters::airDeEss),
        apvts.getRawParameterValue(Parameters::airTone),
        apvts.getRawParameterValue(Parameters::airOutputDb),
        apvts.getRawParameterValue(Parameters::airMix)
    });

    delay.setParameters({
        apvts.getRawParameterValue(Parameters::delayEnabled),
        apvts.getRawParameterValue(Parameters::delayTimeMs),
        apvts.getRawParameterValue(Parameters::delaySyncEnabled),
        apvts.getRawParameterValue(Parameters::delayNoteDivision),
        apvts.getRawParameterValue(Parameters::delayMode),
        apvts.getRawParameterValue(Parameters::delayFeedback),
        apvts.getRawParameterValue(Parameters::delayLeftTime),
        apvts.getRawParameterValue(Parameters::delayRightTime),
        apvts.getRawParameterValue(Parameters::delayLink),
        apvts.getRawParameterValue(Parameters::delayLoFi),
        apvts.getRawParameterValue(Parameters::delayModDepth),
        apvts.getRawParameterValue(Parameters::delayModRate),
        apvts.getRawParameterValue(Parameters::delayHighPassHz),
        apvts.getRawParameterValue(Parameters::delayLowPassHz),
        apvts.getRawParameterValue(Parameters::delayDucking),
        apvts.getRawParameterValue(Parameters::delayFreeze),
        apvts.getRawParameterValue(Parameters::delayStereoWidth),
        apvts.getRawParameterValue(Parameters::delaySend)
    });

    reverb.setParameters({
        apvts.getRawParameterValue(Parameters::reverbEnabled),
        apvts.getRawParameterValue(Parameters::reverbMix),
        apvts.getRawParameterValue(Parameters::reverbPredelayMs),
        apvts.getRawParameterValue(Parameters::reverbDecaySec),
        apvts.getRawParameterValue(Parameters::reverbSize),
        apvts.getRawParameterValue(Parameters::reverbAttack),
        apvts.getRawParameterValue(Parameters::reverbEarly),
        apvts.getRawParameterValue(Parameters::reverbLate),
        apvts.getRawParameterValue(Parameters::reverbLowCutHz),
        apvts.getRawParameterValue(Parameters::reverbHighCutHz),
        apvts.getRawParameterValue(Parameters::reverbLowDampHz),
        apvts.getRawParameterValue(Parameters::reverbHighDampDb),
        apvts.getRawParameterValue(Parameters::reverbDiffusionEarly),
        apvts.getRawParameterValue(Parameters::reverbDiffusionLate),
        apvts.getRawParameterValue(Parameters::reverbModRate),
        apvts.getRawParameterValue(Parameters::reverbModDepth),
        apvts.getRawParameterValue(Parameters::reverbWidth),
        apvts.getRawParameterValue(Parameters::reverbMode),
        apvts.getRawParameterValue(Parameters::reverbColor),
        apvts.getRawParameterValue(Parameters::reverbFreeze),
        apvts.getRawParameterValue(Parameters::reverbDucking),
        apvts.getRawParameterValue(Parameters::reverbTempoSyncPredelay),
        apvts.getRawParameterValue(Parameters::reverbMonoBass)
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
        // Current audio-affecting controls: global gain/bypass, Channel/EQ, FET compressor,
        // Air Exciter, Delay and Reverb in the fixed module order.
        const auto inputGain = juce::Decibels::decibelsToGain(inputGainDbParam != nullptr ? inputGainDbParam->load() : 0.0f);
        const auto outputGain = juce::Decibels::decibelsToGain(outputGainDbParam != nullptr ? outputGainDbParam->load() : 0.0f);
        auto hostBpm = 120.0;

        if (auto* hostPlayHead = getPlayHead())
            if (auto position = hostPlayHead->getPosition())
                if (auto bpm = position->getBpm())
                    hostBpm = *bpm;

        buffer.applyGain(inputGain);
        channel.process(buffer);
        compressor.process(buffer);
        airExciter.process(buffer);
        delay.setHostBpm(hostBpm);
        delay.process(buffer);
        reverb.setHostBpm(hostBpm);
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
