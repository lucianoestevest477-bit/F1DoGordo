#include "F1Dashboard.h"
#include "F1Theme.h"

F1Dashboard::F1Dashboard()
{
    for (auto* knob : { &inputGain, &outputGain, &channelMix, &compMix, &airMix, &delaySend, &reverbSend, &masterWidth })
        addAndMakeVisible(*knob);

    for (auto* button : { &globalBypass, &channelEnabled, &compEnabled, &airEnabled, &delayEnabled, &reverbEnabled })
        addAndMakeVisible(*button);

    for (auto* tab : tabs)
        addAndMakeVisible(*tab);

    moduleLeds[0].setLedColour(F1Theme::green());
    moduleLeds[1].setLedColour(F1Theme::red());
    moduleLeds[2].setLedColour(F1Theme::blue());
    moduleLeds[3].setLedColour(F1Theme::amber());
    moduleLeds[4].setLedColour(F1Theme::green().brighter(0.2f));

    for (auto& led : moduleLeds)
        addAndMakeVisible(led);

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    addAndMakeVisible(gainReductionMeter);
}

void F1Dashboard::setMeterLevels(float input, float output, float gainReduction)
{
    inputMeter.setLevel(input);
    outputMeter.setLevel(output);
    gainReductionMeter.setLevel(gainReduction);
}

void F1Dashboard::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto cockpit = bounds.reduced(70, 38).toFloat();

    g.setColour(juce::Colour(0xff080a0c));
    g.fillRoundedRectangle(cockpit, 44.0f);

    auto inner = cockpit.reduced(18.0f);
    g.setGradientFill(juce::ColourGradient(F1Theme::panel().brighter(0.1f), inner.getCentreX(), inner.getY(),
                                           F1Theme::panel().darker(0.7f), inner.getCentreX(), inner.getBottom(), false));
    g.fillRoundedRectangle(inner, 32.0f);

    g.setColour(F1Theme::red().withAlpha(0.9f));
    g.drawRoundedRectangle(cockpit, 44.0f, 2.0f);

    auto display = inner.withSizeKeepingCentre(inner.getWidth() * 0.42f, inner.getHeight() * 0.34f);
    g.setColour(juce::Colour(0xff050607));
    g.fillRoundedRectangle(display, 8.0f);
    g.setColour(F1Theme::blue().withAlpha(0.55f));
    g.drawRoundedRectangle(display, 8.0f, 1.5f);

    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(26.0f, juce::Font::bold));
    g.drawFittedText("INIT PASS-THROUGH", display.toNearestInt().reduced(10, 16), juce::Justification::centred, 1);

    g.setColour(F1Theme::mutedText());
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawFittedText("INPUT > CHANNEL > FET > AIR > DELAY > REVERB > OUTPUT",
                     display.toNearestInt().withTrimmedTop(display.getHeight() * 0.58f).reduced(10, 0),
                     juce::Justification::centred,
                     1);
}

void F1Dashboard::resized()
{
    auto area = getLocalBounds().reduced(24);
    auto tabArea = area.removeFromTop(48);
    const auto tabWidth = tabArea.getWidth() / static_cast<int>(tabs.size());

    for (auto* tab : tabs)
        tab->setBounds(tabArea.removeFromLeft(tabWidth).reduced(4, 5));

    auto meterArea = area.removeFromRight(170).reduced(8, 32);
    inputMeter.setBounds(meterArea.removeFromLeft(48));
    meterArea.removeFromLeft(8);
    outputMeter.setBounds(meterArea.removeFromLeft(48));
    meterArea.removeFromLeft(8);
    gainReductionMeter.setBounds(meterArea.removeFromLeft(48));

    auto cockpit = area.reduced(50, 40);
    auto topKnobs = cockpit.removeFromTop(150);
    auto bottomKnobs = cockpit.removeFromBottom(150);

    inputGain.setBounds(topKnobs.removeFromLeft(130));
    outputGain.setBounds(topKnobs.removeFromRight(130));
    channelMix.setBounds(topKnobs.removeFromLeft(130));
    masterWidth.setBounds(topKnobs.removeFromRight(130));

    delaySend.setBounds(bottomKnobs.removeFromLeft(130));
    reverbSend.setBounds(bottomKnobs.removeFromRight(130));
    compMix.setBounds(bottomKnobs.removeFromLeft(130));
    airMix.setBounds(bottomKnobs.removeFromRight(130));

    auto switchArea = cockpit.withSizeKeepingCentre(430, 64);
    globalBypass.setBounds(switchArea.removeFromLeft(82).reduced(4, 12));
    channelEnabled.setBounds(switchArea.removeFromLeft(70).reduced(4, 12));
    compEnabled.setBounds(switchArea.removeFromLeft(64).reduced(4, 12));
    airEnabled.setBounds(switchArea.removeFromLeft(58).reduced(4, 12));
    delayEnabled.setBounds(switchArea.removeFromLeft(70).reduced(4, 12));
    reverbEnabled.setBounds(switchArea.removeFromLeft(78).reduced(4, 12));

    auto ledArea = cockpit.withSizeKeepingCentre(260, 24).translated(0, 68);
    for (auto& led : moduleLeds)
        led.setBounds(ledArea.removeFromLeft(52).withSizeKeepingCentre(18, 18));
}
