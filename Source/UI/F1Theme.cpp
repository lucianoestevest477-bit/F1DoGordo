#include "F1Theme.h"

namespace F1Theme
{
    juce::Colour carbon() { return juce::Colour(0xff101214); }
    juce::Colour metal() { return juce::Colour(0xff24282d); }
    juce::Colour panel() { return juce::Colour(0xff171a1e); }
    juce::Colour red() { return juce::Colour(0xffe84b4b); }
    juce::Colour amber() { return juce::Colour(0xffffb84d); }
    juce::Colour green() { return juce::Colour(0xff57d68d); }
    juce::Colour blue() { return juce::Colour(0xff59a7ff); }
    juce::Colour cyan() { return juce::Colour(0xff44d7ff); }
    juce::Colour violet() { return juce::Colour(0xffa77dff); }
    juce::Colour text() { return juce::Colour(0xfff4f6f8); }
    juce::Colour mutedText() { return juce::Colour(0xff9aa3ad); }

    void paintCarbonBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        g.fillAll(juce::Colour(0xff080a0c));

        for (auto y = bounds.getY(); y < bounds.getBottom(); y += 16)
        {
            for (auto x = bounds.getX(); x < bounds.getRight(); x += 16)
            {
                const auto dark = ((x / 16 + y / 16) % 2) == 0;
                g.setColour(dark ? juce::Colour(0x24232931) : juce::Colour(0x22101418));
                g.fillRect(x, y, 16, 16);
            }
        }

        g.setColour(juce::Colour(0x18000000));
        for (auto x = bounds.getX() - bounds.getHeight(); x < bounds.getRight(); x += 28)
            g.drawLine(static_cast<float>(x),
                       static_cast<float>(bounds.getBottom()),
                       static_cast<float>(x + bounds.getHeight()),
                       static_cast<float>(bounds.getY()),
                       1.0f);

        g.setColour(juce::Colour(0x0f7f8d99));
        for (auto x = bounds.getX(); x < bounds.getRight() + bounds.getHeight(); x += 34)
            g.drawLine(static_cast<float>(x),
                       static_cast<float>(bounds.getY()),
                       static_cast<float>(x - bounds.getHeight()),
                       static_cast<float>(bounds.getBottom()),
                       1.0f);

        g.setGradientFill(juce::ColourGradient(juce::Colour(0x88000000),
                                               static_cast<float>(bounds.getCentreX()),
                                               static_cast<float>(bounds.getY()),
                                               juce::Colour(0x33000000),
                                               static_cast<float>(bounds.getCentreX()),
                                               static_cast<float>(bounds.getBottom()),
                                               false));
        g.fillRect(bounds);

        const auto glow = juce::Rectangle<float>(bounds.toFloat()).reduced(bounds.getWidth() * 0.16f,
                                                                           bounds.getHeight() * 0.18f);
        g.setGradientFill(juce::ColourGradient(blue().withAlpha(0.10f),
                                               glow.getCentreX(),
                                               glow.getCentreY(),
                                               juce::Colours::transparentBlack,
                                               glow.getRight(),
                                               glow.getBottom(),
                                               true));
        g.fillEllipse(glow);
    }
}
