#include "GordoButton.h"
#include "F1Theme.h"

GordoButton::GordoButton(const juce::String& text)
    : juce::Button(text)
{
    setClickingTogglesState(true);
}

void GordoButton::paintButton(juce::Graphics& g, bool highlighted, bool down)
{
    const auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    const auto active = getToggleState();
    const auto base = active ? F1Theme::blue() : F1Theme::metal();

    g.setColour(juce::Colour(0xff050607));
    g.fillRoundedRectangle(bounds, 5.0f);

    g.setGradientFill(juce::ColourGradient(base.brighter(highlighted ? 0.35f : 0.12f), bounds.getX(), bounds.getY(),
                                           base.darker(down ? 0.35f : 0.6f), bounds.getX(), bounds.getBottom(), false));
    g.fillRoundedRectangle(bounds.reduced(down ? 3.0f : 2.0f), 4.0f);

    g.setColour(active ? F1Theme::text() : F1Theme::mutedText());
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawFittedText(getButtonText(), getLocalBounds().reduced(6, 2), juce::Justification::centred, 1);
}
