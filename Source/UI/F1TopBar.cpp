#include "F1TopBar.h"
#include "F1Theme.h"

F1TopBar::F1TopBar()
{
    presetLabel.setText("Factory Init", juce::dontSendNotification);
    qualityLabel.setText("QUALITY: HQ", juce::dontSendNotification);
    cpuLabel.setText("CPU 0%", juce::dontSendNotification);

    for (auto* label : { &presetLabel, &qualityLabel, &cpuLabel })
    {
        label->setColour(juce::Label::textColourId, F1Theme::text());
        label->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label->setJustificationType(juce::Justification::centred);
        label->setFont(juce::FontOptions(13.0f, juce::Font::bold));
        addAndMakeVisible(*label);
    }

    addAndMakeVisible(abButton);
    addAndMakeVisible(saveButton);
}

void F1TopBar::setCpuText(const juce::String& text)
{
    cpuLabel.setText(text, juce::dontSendNotification);
}

void F1TopBar::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto inset = bounds.reduced(18.0f, 8.0f);
    juce::Path bridge;
    bridge.startNewSubPath(inset.getX() + 58.0f, inset.getY() + 2.0f);
    bridge.lineTo(inset.getRight() - 58.0f, inset.getY() + 2.0f);
    bridge.lineTo(inset.getRight() - 18.0f, inset.getCentreY());
    bridge.lineTo(inset.getRight() - 74.0f, inset.getBottom() - 3.0f);
    bridge.lineTo(inset.getX() + 74.0f, inset.getBottom() - 3.0f);
    bridge.lineTo(inset.getX() + 18.0f, inset.getCentreY());
    bridge.closeSubPath();

    g.setColour(juce::Colours::black.withAlpha(0.68f));
    g.fillPath(bridge, juce::AffineTransform::translation(0.0f, 7.0f));
    g.setColour(juce::Colour(0xff050607));
    g.fillPath(bridge);
    g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.14f), inset.getX(), inset.getY(),
                                           juce::Colour(0xff020304), inset.getX(), inset.getBottom(), false));
    g.fillPath(bridge, juce::AffineTransform::scale(0.985f, 0.88f, inset.getCentreX(), inset.getCentreY()));

    for (auto x = inset.getX() + 94.0f; x < inset.getRight() - 94.0f; x += 18.0f)
    {
        g.setColour(juce::Colours::white.withAlpha(0.045f));
        g.drawVerticalLine(juce::roundToInt(x), inset.getY() + 8.0f, inset.getBottom() - 9.0f);
    }

    g.setColour(F1Theme::cyan().withAlpha(0.92f));
    g.fillRoundedRectangle(juce::Rectangle<float>(inset.getX() + 8.0f, inset.getBottom() - 5.0f,
                                                  inset.getWidth() - 16.0f, 2.0f),
                           1.0f);
    g.setColour(F1Theme::cyan().withAlpha(0.34f));
    g.strokePath(bridge, juce::PathStrokeType(1.7f));

    for (auto x = inset.getX() + 330.0f; x < inset.getRight() - 80.0f; x += 26.0f)
    {
        auto led = juce::Rectangle<float>(5.0f, 5.0f).withCentre(juce::Point<float>(x, inset.getY() + 13.0f));
        g.setColour(F1Theme::green().withAlpha(0.20f));
        g.fillEllipse(led.expanded(4.0f));
        g.setColour(F1Theme::green().withAlpha(0.74f));
        g.fillEllipse(led);
    }

    for (auto screwX : { inset.getX() + 48.0f, inset.getRight() - 48.0f })
    {
        auto screw = juce::Rectangle<float>(12.0f, 12.0f).withCentre(juce::Point<float>(screwX, inset.getCentreY()));
        g.setColour(juce::Colour(0xff030405));
        g.fillEllipse(screw.expanded(2.0f));
        g.setColour(F1Theme::metal().brighter(0.35f));
        g.fillEllipse(screw);
        g.setColour(juce::Colours::black.withAlpha(0.82f));
        g.drawLine(screw.getX() + 3.0f, screw.getCentreY(), screw.getRight() - 3.0f, screw.getCentreY(), 1.4f);
    }

    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    g.drawFittedText("F1 do Gordo", 30, 10, 280, 34, juce::Justification::centredLeft, 1);

    g.setColour(F1Theme::cyan().withAlpha(0.82f));
    g.setFont(juce::FontOptions(12.5f, juce::Font::bold));
    g.drawFittedText("Cockpit FX Rack", 32, 43, 200, 18, juce::Justification::centredLeft, 1);

    g.setColour(F1Theme::mutedText());
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawFittedText("PRESET", 322, 12, 64, 16, juce::Justification::centredLeft, 1);
}

void F1TopBar::resized()
{
    auto area = getLocalBounds().reduced(20, 10);
    area.removeFromLeft(360);

    presetLabel.setBounds(area.removeFromLeft(210).reduced(0, 9));
    area.removeFromLeft(14);
    abButton.setBounds(area.removeFromLeft(64).reduced(0, 7));
    area.removeFromLeft(8);
    saveButton.setBounds(area.removeFromLeft(78).reduced(0, 7));
    area.removeFromLeft(28);
    qualityLabel.setBounds(area.removeFromLeft(132).reduced(0, 8));
    cpuLabel.setBounds(area.removeFromRight(92).reduced(0, 8));
}
