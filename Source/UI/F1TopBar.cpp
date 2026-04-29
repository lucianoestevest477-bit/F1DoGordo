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
    g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.18f), bounds.getCentreX(), bounds.getY(),
                                           juce::Colour(0xff07090c), bounds.getCentreX(), bounds.getBottom(), false));
    g.fillRect(bounds);

    auto inset = bounds.reduced(14.0f, 10.0f);
    g.setColour(juce::Colour(0xff050607));
    g.fillRoundedRectangle(inset, 10.0f);
    g.setGradientFill(juce::ColourGradient(F1Theme::panel().brighter(0.14f), inset.getX(), inset.getY(),
                                           F1Theme::panel().darker(0.7f), inset.getX(), inset.getBottom(), false));
    g.fillRoundedRectangle(inset.reduced(2.0f), 8.0f);

    g.setColour(F1Theme::red().withAlpha(0.85f));
    g.fillRoundedRectangle(juce::Rectangle<float>(inset.getX() + 8.0f, inset.getBottom() - 5.0f,
                                                  inset.getWidth() - 16.0f, 2.0f),
                           1.0f);

    for (auto x = inset.getX() + 330.0f; x < inset.getRight() - 80.0f; x += 26.0f)
    {
        g.setColour(juce::Colour(0x1affffff));
        g.drawVerticalLine(juce::roundToInt(x), inset.getY() + 7.0f, inset.getBottom() - 7.0f);
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
