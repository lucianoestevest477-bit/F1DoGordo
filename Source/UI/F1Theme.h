#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace F1Theme
{
    juce::Colour carbon();
    juce::Colour metal();
    juce::Colour panel();
    juce::Colour red();
    juce::Colour amber();
    juce::Colour green();
    juce::Colour blue();
    juce::Colour text();
    juce::Colour mutedText();

    void paintCarbonBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
}
