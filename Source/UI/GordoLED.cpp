#include "GordoLED.h"
#include "F1Theme.h"

GordoLED::GordoLED()
    : colour(F1Theme::green())
{
}

void GordoLED::setLedColour(juce::Colour newColour)
{
    colour = newColour;
    repaint();
}

void GordoLED::setOn(bool shouldBeOn)
{
    on = shouldBeOn;
    repaint();
}

void GordoLED::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    const auto glow = on ? colour : F1Theme::metal().darker(0.72f);

    g.setColour(on ? glow.withAlpha(0.34f) : juce::Colours::black.withAlpha(0.26f));
    g.fillEllipse(bounds.expanded(on ? 7.0f : 3.0f));
    g.setColour(juce::Colour(0xff050607));
    g.fillEllipse(bounds.expanded(1.5f));
    g.setGradientFill(juce::ColourGradient(glow.brighter(on ? 0.75f : 0.10f), bounds.getCentreX(), bounds.getY(),
                                           glow.darker(on ? 0.58f : 0.30f), bounds.getCentreX(), bounds.getBottom(), false));
    g.fillEllipse(bounds.reduced(on ? 0.0f : 1.0f));
    g.setColour(juce::Colours::white.withAlpha(on ? 0.28f : 0.06f));
    g.fillEllipse(bounds.withSizeKeepingCentre(bounds.getWidth() * 0.38f, bounds.getHeight() * 0.30f)
                      .translated(-bounds.getWidth() * 0.14f, -bounds.getHeight() * 0.18f));
    g.setColour(juce::Colour(0xaa000000));
    g.drawEllipse(bounds, 1.0f);
}
