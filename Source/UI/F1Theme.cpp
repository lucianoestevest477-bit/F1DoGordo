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
    juce::Colour text() { return juce::Colour(0xfff4f6f8); }
    juce::Colour mutedText() { return juce::Colour(0xff9aa3ad); }

    void paintCarbonBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        g.fillAll(carbon());

        for (auto y = bounds.getY(); y < bounds.getBottom(); y += 18)
        {
            for (auto x = bounds.getX(); x < bounds.getRight(); x += 18)
            {
                const auto dark = ((x / 18 + y / 18) % 2) == 0;
                g.setColour(dark ? juce::Colour(0x221f2428) : juce::Colour(0x22101418));
                g.fillRect(x, y, 18, 18);
            }
        }

        g.setGradientFill(juce::ColourGradient(juce::Colour(0x66000000),
                                               static_cast<float>(bounds.getCentreX()),
                                               static_cast<float>(bounds.getY()),
                                               juce::Colour(0x22000000),
                                               static_cast<float>(bounds.getCentreX()),
                                               static_cast<float>(bounds.getBottom()),
                                               false));
        g.fillRect(bounds);
    }
}
