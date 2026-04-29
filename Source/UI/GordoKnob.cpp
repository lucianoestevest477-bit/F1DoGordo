#include "GordoKnob.h"
#include "F1Theme.h"

#include <cmath>

namespace
{
    juce::Colour accentForLabel(const juce::String& label)
    {
        const auto upper = label.toUpperCase();

        if (upper.contains("COMP") || upper.contains("PUNCH") || upper.contains("DRIVE") || upper.contains("THRESH"))
            return F1Theme::red();

        if (upper.contains("AIR") || upper.contains("HIGH"))
            return F1Theme::blue();

        if (upper.contains("DELAY") || upper.contains("ECHO") || upper.contains("TIME") || upper.contains("FEEDBACK"))
            return F1Theme::amber();

        if (upper.contains("REVERB") || upper.contains("SPACE") || upper.contains("WIDTH"))
            return F1Theme::cyan();

        if (upper.contains("LOW") || upper.contains("TONE") || upper.contains("CHANNEL"))
            return F1Theme::green();

        return F1Theme::blue();
    }
}

GordoKnob::GordoKnob(const juce::String& labelText)
    : juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow),
      label(labelText)
{
    setRange(0.0, 1.0);
    setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    setColour(juce::Slider::textBoxTextColourId, F1Theme::text());
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void GordoKnob::setLabelText(const juce::String& newLabelText)
{
    label = newLabelText;
    repaint();
}

void GordoKnob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    auto labelArea = bounds.removeFromTop(31.0f);
    auto valueArea = bounds.removeFromBottom(25.0f);
    auto knobSize = juce::jmax(34.0f, juce::jmin(bounds.getWidth() - 16.0f, bounds.getHeight() - 8.0f));
    auto knobArea = bounds.reduced(8.0f, 2.0f).withSizeKeepingCentre(knobSize, knobSize);

    const auto radius = knobArea.getWidth() * 0.5f;
    const auto centre = knobArea.getCentre();
    const auto startAngle = juce::MathConstants<float>::pi * 1.25f;
    const auto endAngle = juce::MathConstants<float>::pi * 2.75f;
    const auto range = getMaximum() - getMinimum();
    const auto proportion = range > 0.0 ? static_cast<float>((getValue() - getMinimum()) / range) : 0.0f;
    const auto angle = juce::jmap(proportion, startAngle, endAngle);
    const auto accent = accentForLabel(label);

    g.setColour(juce::Colour(0x88000000));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 7.0f);
    g.setColour(juce::Colour(0x18ffffff));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.5f), 7.0f, 1.0f);

    g.setColour(juce::Colour(0xff07090b));
    g.fillEllipse(knobArea.expanded(7.0f));

    for (auto tick = 0; tick <= 10; ++tick)
    {
        const auto tickAngle = juce::jmap(static_cast<float>(tick) / 10.0f, startAngle, endAngle);
        const auto tickLength = tick % 5 == 0 ? 6.0f : 3.5f;
        const auto outer = juce::Point<float>(centre.x + std::sin(tickAngle) * (radius + 10.0f),
                                              centre.y - std::cos(tickAngle) * (radius + 10.0f));
        const auto inner = juce::Point<float>(centre.x + std::sin(tickAngle) * (radius + 10.0f - tickLength),
                                              centre.y - std::cos(tickAngle) * (radius + 10.0f - tickLength));
        g.setColour(tick <= juce::roundToInt(proportion * 10.0f) ? accent.withAlpha(0.72f)
                                                                 : F1Theme::mutedText().withAlpha(0.22f));
        g.drawLine(outer.x, outer.y, inner.x, inner.y, 1.3f);
    }

    g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.25f), centre.x, knobArea.getY(),
                                           F1Theme::metal().darker(0.55f), centre.x, knobArea.getBottom(), false));
    g.fillEllipse(knobArea);
    g.setColour(juce::Colour(0xff050607));
    g.fillEllipse(knobArea.reduced(radius * 0.33f));

    g.setColour(accent.withAlpha(0.95f));
    juce::Path arc;
    arc.addCentredArc(centre.x, centre.y, radius + 5.0f, radius + 5.0f, 0.0f, startAngle, angle, true);
    g.strokePath(arc, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    g.setColour(accent.withAlpha(0.16f));
    g.fillEllipse(knobArea.reduced(radius * 0.18f));

    juce::Path pointer;
    pointer.addRoundedRectangle(-2.5f, -radius + 9.0f, 5.0f, radius * 0.54f, 2.0f);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
    g.setColour(F1Theme::text());
    g.fillPath(pointer);

    g.setColour(juce::Colour(0x88000000));
    g.drawEllipse(knobArea, 1.4f);

    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(12.5f, juce::Font::bold));
    g.drawFittedText(label, labelArea.toNearestInt(), juce::Justification::centred, 2);

    auto valueBox = valueArea.reduced(8.0f, 2.0f);
    g.setColour(juce::Colour(0xdd050607));
    g.fillRoundedRectangle(valueBox, 4.0f);
    g.setColour(accent.withAlpha(0.45f));
    g.drawRoundedRectangle(valueBox, 4.0f, 1.0f);
    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    g.drawFittedText(getTextFromValue(getValue()), valueBox.toNearestInt().reduced(2, 0), juce::Justification::centred, 1);
}
