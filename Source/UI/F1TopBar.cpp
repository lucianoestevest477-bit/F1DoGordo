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
        label->setJustificationType(juce::Justification::centred);
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
    g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.18f), bounds.getX(), bounds.getY(),
                                           F1Theme::carbon(), bounds.getX(), bounds.getBottom(), false));
    g.fillRect(bounds);

    g.setColour(F1Theme::red());
    g.fillRect(0, getHeight() - 3, getWidth(), 3);

    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    g.drawFittedText("F1 do Gordo", 24, 0, 280, getHeight(), juce::Justification::centredLeft, 1);
}

void F1TopBar::resized()
{
    auto area = getLocalBounds().reduced(20, 10);
    area.removeFromLeft(300);

    presetLabel.setBounds(area.removeFromLeft(220));
    area.removeFromLeft(10);
    abButton.setBounds(area.removeFromLeft(64).reduced(0, 6));
    area.removeFromLeft(8);
    saveButton.setBounds(area.removeFromLeft(78).reduced(0, 6));
    area.removeFromLeft(20);
    qualityLabel.setBounds(area.removeFromLeft(130));
    cpuLabel.setBounds(area.removeFromRight(90));
}
