#include "GordoButton.h"
#include "F1Theme.h"

namespace
{
    juce::Colour accentForButton(const juce::String& text)
    {
        const auto upper = text.toUpperCase();

        if (upper.contains("COMP") || upper.contains("BYPASS") || upper == "A/B")
            return F1Theme::red();

        if (upper.contains("AIR"))
            return F1Theme::blue();

        if (upper.contains("DELAY") || upper.contains("ECHO") || upper.contains("SYNC") || upper.contains("FREEZE"))
            return F1Theme::amber();

        if (upper.contains("REVERB") || upper.contains("SPACE") || upper.contains("MONO"))
            return F1Theme::cyan();

        if (upper.contains("CHANNEL") || upper.contains("SAVE") || upper.contains("ON"))
            return F1Theme::green();

        return F1Theme::blue();
    }
}

GordoButton::GordoButton(const juce::String& text)
    : juce::Button(text)
{
    setClickingTogglesState(true);
}

void GordoButton::paintButton(juce::Graphics& g, bool highlighted, bool down)
{
    const auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    const auto active = getToggleState();
    const auto accent = accentForButton(getButtonText());
    const auto base = active ? accent : F1Theme::metal();

    g.setColour(juce::Colour(0xff050607));
    g.fillRoundedRectangle(bounds, 7.0f);

    if (active)
    {
        g.setColour(accent.withAlpha(highlighted ? 0.34f : 0.22f));
        g.fillRoundedRectangle(bounds.expanded(4.0f), 9.0f);
    }

    g.setGradientFill(juce::ColourGradient(base.brighter(highlighted ? 0.35f : 0.12f), bounds.getX(), bounds.getY(),
                                           base.darker(down ? 0.35f : 0.6f), bounds.getX(), bounds.getBottom(), false));
    g.fillRoundedRectangle(bounds.reduced(down ? 3.0f : 2.0f), 6.0f);

    auto cap = bounds.reduced(7.0f, 5.0f).removeFromTop(3.0f);
    g.setColour(active ? juce::Colours::white.withAlpha(0.28f) : juce::Colours::white.withAlpha(0.08f));
    g.fillRoundedRectangle(cap, 1.5f);

    auto led = bounds.withSizeKeepingCentre(8.0f, 8.0f).withX(bounds.getRight() - 14.0f).withY(bounds.getY() + 7.0f);
    g.setColour(active ? accent.withAlpha(0.28f) : juce::Colours::black.withAlpha(0.35f));
    g.fillEllipse(led.expanded(3.0f));
    g.setColour(active ? accent.brighter(0.45f) : F1Theme::metal().darker(0.6f));
    g.fillEllipse(led);
    g.setColour(juce::Colour(0x88000000));
    g.drawEllipse(led, 1.0f);

    g.setColour(active ? F1Theme::text() : F1Theme::mutedText());
    g.setFont(juce::FontOptions(12.5f, juce::Font::bold));
    g.drawFittedText(getButtonText(), getLocalBounds().reduced(8, 3), juce::Justification::centred, 1);
}
