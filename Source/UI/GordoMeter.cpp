#include "GordoMeter.h"
#include "F1Theme.h"

GordoMeter::GordoMeter(const juce::String& labelText)
    : label(labelText)
{
}

void GordoMeter::setLevel(float newLevel)
{
    level = juce::jlimit(0.0f, 1.0f, newLevel);
    repaint();
}

void GordoMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(3);
    auto labelArea = bounds.removeFromBottom(18);
    auto meterArea = bounds.toFloat();

    g.setColour(juce::Colour(0xff050607));
    g.fillRoundedRectangle(meterArea, 3.0f);

    auto fill = meterArea.reduced(4.0f);
    if (reductionMode)
        fill.removeFromBottom(fill.getHeight() * (1.0f - level));
    else
        fill.removeFromTop(fill.getHeight() * (1.0f - level));

    const auto fillStart = reductionMode ? F1Theme::red() : F1Theme::green();
    const auto fillEnd = reductionMode ? F1Theme::amber() : (level > 0.75f ? F1Theme::red() : F1Theme::amber());
    g.setGradientFill(juce::ColourGradient(fillStart, fill.getX(), fill.getBottom(),
                                           fillEnd, fill.getX(), fill.getY(), false));
    g.fillRoundedRectangle(fill, 2.0f);

    g.setColour(F1Theme::mutedText());
    g.drawRoundedRectangle(meterArea, 3.0f, 1.0f);
    g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    g.drawFittedText(label, labelArea, juce::Justification::centred, 1);
}
