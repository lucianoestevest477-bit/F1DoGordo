#include "Metering.h"

#include <cmath>

void Metering::reset() noexcept
{
    peak = 0.0f;
    rms = 0.0f;
}

void Metering::update(const juce::AudioBuffer<float>& buffer) noexcept
{
    auto newPeak = 0.0f;
    double squareSum = 0.0;
    auto sampleCount = 0;

    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        const auto* data = buffer.getReadPointer(channel);

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const auto value = std::abs(data[sample]);
            newPeak = juce::jmax(newPeak, value);
            squareSum += static_cast<double>(data[sample]) * static_cast<double>(data[sample]);
            ++sampleCount;
        }
    }

    peak = newPeak;
    rms = sampleCount > 0 ? static_cast<float>(std::sqrt(squareSum / static_cast<double>(sampleCount))) : 0.0f;
}
