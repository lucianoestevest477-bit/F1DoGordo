#include "GordoKnob.h"
#include "F1Theme.h"

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
    auto labelArea = bounds.removeFromTop(34.0f);
    auto valueArea = bounds.removeFromBottom(26.0f);
    auto knobSize = juce::jmax(34.0f, juce::jmin(bounds.getWidth() - 12.0f, bounds.getHeight() - 6.0f));
    auto knobArea = bounds.reduced(6.0f, 2.0f).withSizeKeepingCentre(knobSize, knobSize);

    const auto radius = knobArea.getWidth() * 0.5f;
    const auto centre = knobArea.getCentre();
    const auto startAngle = juce::MathConstants<float>::pi * 1.25f;
    const auto endAngle = juce::MathConstants<float>::pi * 2.75f;
    const auto proportion = static_cast<float>((getValue() - getMinimum()) / (getMaximum() - getMinimum()));
    const auto angle = juce::jmap(proportion, startAngle, endAngle);

    g.setColour(juce::Colour(0xff07090b));
    g.fillEllipse(knobArea.expanded(5.0f));

    g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.25f), centre.x, knobArea.getY(),
                                           F1Theme::metal().darker(0.55f), centre.x, knobArea.getBottom(), false));
    g.fillEllipse(knobArea);

    g.setColour(F1Theme::blue().withAlpha(0.85f));
    juce::Path arc;
    arc.addCentredArc(centre.x, centre.y, radius + 4.0f, radius + 4.0f, 0.0f, startAngle, angle, true);
    g.strokePath(arc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    pointer.addRectangle(-2.0f, -radius + 10.0f, 4.0f, radius * 0.5f);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
    g.setColour(F1Theme::text());
    g.fillPath(pointer);

    g.setColour(juce::Colour(0x88000000));
    g.drawEllipse(knobArea, 1.4f);

    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawFittedText(label, labelArea.toNearestInt(), juce::Justification::centred, 2);

    auto valueBox = valueArea.reduced(8.0f, 2.0f);
    g.setColour(juce::Colour(0xaa050607));
    g.fillRoundedRectangle(valueBox, 4.0f);
    g.setColour(F1Theme::blue().withAlpha(0.35f));
    g.drawRoundedRectangle(valueBox, 4.0f, 1.0f);
    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    g.drawFittedText(getTextFromValue(getValue()), valueBox.toNearestInt().reduced(2, 0), juce::Justification::centred, 1);
}
