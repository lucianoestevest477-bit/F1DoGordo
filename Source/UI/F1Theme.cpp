#include "F1Theme.h"

namespace F1Theme
{
    juce::Colour carbon() { return juce::Colour(0xff07090b); }
    juce::Colour metal() { return juce::Colour(0xff171b20); }
    juce::Colour panel() { return juce::Colour(0xff0c0f13); }
    juce::Colour red() { return juce::Colour(0xffff3045); }
    juce::Colour amber() { return juce::Colour(0xffffb000); }
    juce::Colour green() { return juce::Colour(0xff22ff91); }
    juce::Colour blue() { return juce::Colour(0xff1e9bff); }
    juce::Colour cyan() { return juce::Colour(0xff20f2ff); }
    juce::Colour violet() { return juce::Colour(0xffb05cff); }
    juce::Colour text() { return juce::Colour(0xfff4f6f8); }
    juce::Colour mutedText() { return juce::Colour(0xff9aa3ad); }

    void paintCarbonBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        g.fillAll(juce::Colour(0xff020304));

        for (auto y = bounds.getY(); y < bounds.getBottom(); y += 14)
        {
            for (auto x = bounds.getX(); x < bounds.getRight(); x += 14)
            {
                const auto dark = ((x / 14 + y / 14) % 2) == 0;
                g.setColour(dark ? juce::Colour(0x20161b22) : juce::Colour(0x16070a0f));
                g.fillRect(x, y, 14, 14);
            }
        }

        g.setColour(juce::Colour(0x20000000));
        for (auto x = bounds.getX() - bounds.getHeight(); x < bounds.getRight(); x += 22)
            g.drawLine(static_cast<float>(x),
                       static_cast<float>(bounds.getBottom()),
                       static_cast<float>(x + bounds.getHeight()),
                       static_cast<float>(bounds.getY()),
                       1.0f);

        g.setColour(juce::Colour(0x127a8996));
        for (auto x = bounds.getX(); x < bounds.getRight() + bounds.getHeight(); x += 26)
            g.drawLine(static_cast<float>(x),
                       static_cast<float>(bounds.getY()),
                       static_cast<float>(x - bounds.getHeight()),
                       static_cast<float>(bounds.getBottom()),
                       1.0f);

        g.setGradientFill(juce::ColourGradient(juce::Colour(0x88000000),
                                               static_cast<float>(bounds.getCentreX()),
                                               static_cast<float>(bounds.getY()),
                                               juce::Colour(0xbb000000),
                                               static_cast<float>(bounds.getCentreX()),
                                               static_cast<float>(bounds.getBottom()),
                                               false));
        g.fillRect(bounds);

        const auto glow = juce::Rectangle<float>(bounds.toFloat()).reduced(bounds.getWidth() * 0.16f,
                                                                           bounds.getHeight() * 0.18f);
        g.setGradientFill(juce::ColourGradient(cyan().withAlpha(0.12f),
                                               glow.getCentreX(),
                                               glow.getCentreY(),
                                               juce::Colours::transparentBlack,
                                               glow.getRight(),
                                               glow.getBottom(),
                                               true));
        g.fillEllipse(glow);

        g.setGradientFill(juce::ColourGradient(juce::Colours::transparentBlack,
                                               static_cast<float>(bounds.getCentreX()),
                                               static_cast<float>(bounds.getCentreY()),
                                               juce::Colour(0xdd000000),
                                               static_cast<float>(bounds.getX()),
                                               static_cast<float>(bounds.getY()),
                                               true));
        g.fillRect(bounds);
    }
}
