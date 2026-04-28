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
    const auto glow = on ? colour : F1Theme::metal().darker(0.5f);

    g.setColour(glow.withAlpha(on ? 0.25f : 0.05f));
    g.fillEllipse(bounds.expanded(5.0f));
    g.setGradientFill(juce::ColourGradient(glow.brighter(0.6f), bounds.getCentreX(), bounds.getY(),
                                           glow.darker(0.65f), bounds.getCentreX(), bounds.getBottom(), false));
    g.fillEllipse(bounds);
    g.setColour(juce::Colour(0xaa000000));
    g.drawEllipse(bounds, 1.0f);
}
