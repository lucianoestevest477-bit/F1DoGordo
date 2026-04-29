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
    auto labelArea = bounds.removeFromBottom(22);
    auto meterArea = bounds.toFloat();

    g.setColour(juce::Colour(0xff050607));
    g.fillRoundedRectangle(meterArea.expanded(2.0f), 5.0f);

    g.setGradientFill(juce::ColourGradient(F1Theme::panel().brighter(0.06f), meterArea.getX(), meterArea.getY(),
                                           F1Theme::panel().darker(0.70f), meterArea.getX(), meterArea.getBottom(), false));
    g.fillRoundedRectangle(meterArea, 4.0f);

    for (auto tick = 1; tick < 8; ++tick)
    {
        const auto y = meterArea.getY() + meterArea.getHeight() * static_cast<float>(tick) / 8.0f;
        g.setColour(tick == 4 ? juce::Colour(0x33ffffff) : juce::Colour(0x1affffff));
        g.drawHorizontalLine(juce::roundToInt(y), meterArea.getX() + 5.0f, meterArea.getRight() - 5.0f);
    }

    auto fill = meterArea.reduced(4.0f);
    if (reductionMode)
        fill.removeFromBottom(fill.getHeight() * (1.0f - level));
    else
        fill.removeFromTop(fill.getHeight() * (1.0f - level));

    const auto fillStart = reductionMode ? F1Theme::violet() : F1Theme::green();
    const auto fillEnd = reductionMode ? F1Theme::red() : (level > 0.78f ? F1Theme::red() : F1Theme::amber());
    g.setGradientFill(juce::ColourGradient(fillStart, fill.getX(), fill.getBottom(),
                                           fillEnd, fill.getX(), fill.getY(), false));
    g.fillRoundedRectangle(fill, 2.0f);

    g.setColour(reductionMode ? F1Theme::violet().withAlpha(0.58f) : F1Theme::green().withAlpha(0.42f));
    g.drawRoundedRectangle(meterArea, 4.0f, 1.2f);

    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    g.drawFittedText(label, labelArea, juce::Justification::centred, 1);
}
