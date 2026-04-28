#include "F1Dashboard.h"
#include "F1Theme.h"

F1Dashboard::F1Dashboard()
{
    for (auto* knob : { &inputGain, &outputGain, &channelMix, &compMix, &airMix, &delaySend, &reverbSend, &masterWidth })
        addAndMakeVisible(*knob);

    for (auto* knob : { &channelInputTrim, &channelHighPass, &channelLowPass, &channelLowGain, &channelLowFreq,
                        &channelLowMidGain, &channelLowMidFreq, &channelHighMidGain, &channelHighMidFreq,
                        &channelHighGain, &channelHighFreq, &channelDrive, &channelPageMix })
        addAndMakeVisible(*knob);

    for (auto* button : { &globalBypass, &channelEnabled, &compEnabled, &airEnabled, &delayEnabled, &reverbEnabled })
        addAndMakeVisible(*button);

    addAndMakeVisible(channelPhaseInvert);

    for (auto* tab : tabs)
    {
        addAndMakeVisible(*tab);
        tab->setRadioGroupId(1001, juce::dontSendNotification);
    }

    tabGlobal.onClick = [this] { setPage(Page::global); };
    tabChannel.onClick = [this] { setPage(Page::channel); };
    tabComp.onClick = [this] { setPage(Page::global); };
    tabAir.onClick = [this] { setPage(Page::global); };
    tabDelay.onClick = [this] { setPage(Page::global); };
    tabReverb.onClick = [this] { setPage(Page::global); };
    tabRouting.onClick = [this] { setPage(Page::global); };
    tabMeters.onClick = [this] { setPage(Page::global); };

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

    setPage(Page::global);
}

void F1Dashboard::setMeterLevels(float input, float output, float gainReduction)
{
    inputMeter.setLevel(input);
    outputMeter.setLevel(output);
    gainReductionMeter.setLevel(gainReduction);
}

void F1Dashboard::setModuleStates(bool channelOn, bool compOn, bool airOn, bool delayOn, bool reverbOn)
{
    moduleLeds[0].setOn(channelOn);
    moduleLeds[1].setOn(compOn);
    moduleLeds[2].setOn(airOn);
    moduleLeds[3].setOn(delayOn);
    moduleLeds[4].setOn(reverbOn);
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

    auto display = activePage == Page::channel
                       ? inner.withSizeKeepingCentre(inner.getWidth() * 0.44f, 58.0f).withY(inner.getY() + 10.0f)
                       : inner.withSizeKeepingCentre(inner.getWidth() * 0.42f, inner.getHeight() * 0.34f);
    g.setColour(juce::Colour(0xff050607));
    g.fillRoundedRectangle(display, 8.0f);
    g.setColour(F1Theme::blue().withAlpha(0.55f));
    g.drawRoundedRectangle(display, 8.0f, 1.5f);

    const auto title = activePage == Page::channel ? "CHANNEL EQ" : "GLOBAL CONTROL";
    const auto route = activePage == Page::channel
                           ? "TRIM > HPF > 4-BAND EQ > DRIVE > MIX"
                           : "INPUT > CHANNEL > FET > AIR > DELAY > REVERB > OUTPUT";

    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(26.0f, juce::Font::bold));
    g.drawFittedText(title, display.toNearestInt().reduced(10, 16), juce::Justification::centred, 1);

    g.setColour(F1Theme::mutedText());
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    const auto routingTextTop = juce::roundToInt(display.getHeight() * 0.58f);
    g.drawFittedText(route,
                     display.toNearestInt().withTrimmedTop(routingTextTop).reduced(10, 0),
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

    if (activePage == Page::channel)
        layoutChannelPage(cockpit);
    else
        layoutGlobalPage(cockpit);

    auto ledArea = cockpit.withSizeKeepingCentre(260, 24).translated(0, 68);
    for (auto& led : moduleLeds)
        led.setBounds(ledArea.removeFromLeft(52).withSizeKeepingCentre(18, 18));
}

void F1Dashboard::setPage(Page newPage)
{
    activePage = newPage;
    tabGlobal.setToggleState(activePage == Page::global, juce::dontSendNotification);
    tabChannel.setToggleState(activePage == Page::channel, juce::dontSendNotification);
    updateControlVisibility();
    resized();
    repaint();
}

void F1Dashboard::updateControlVisibility()
{
    const auto onGlobal = activePage == Page::global;
    const auto onChannel = activePage == Page::channel;

    for (auto* knob : { &inputGain, &outputGain, &channelMix, &compMix, &airMix, &delaySend, &reverbSend, &masterWidth })
        knob->setVisible(onGlobal);

    for (auto* knob : { &channelInputTrim, &channelHighPass, &channelLowPass, &channelLowGain, &channelLowFreq,
                        &channelLowMidGain, &channelLowMidFreq, &channelHighMidGain, &channelHighMidFreq,
                        &channelHighGain, &channelHighFreq, &channelDrive, &channelPageMix })
        knob->setVisible(onChannel);

    globalBypass.setVisible(onGlobal);
    channelEnabled.setVisible(onGlobal);
    compEnabled.setVisible(onGlobal);
    airEnabled.setVisible(onGlobal);
    delayEnabled.setVisible(onGlobal);
    reverbEnabled.setVisible(onGlobal);
    channelPhaseInvert.setVisible(onChannel);
}

void F1Dashboard::layoutGlobalPage(juce::Rectangle<int> cockpit)
{
    auto topKnobs = cockpit.removeFromTop(150);
    auto bottomKnobs = cockpit.removeFromBottom(150);

    inputGain.setBounds(topKnobs.removeFromLeft(136));
    outputGain.setBounds(topKnobs.removeFromRight(136));
    channelMix.setBounds(topKnobs.removeFromLeft(136));
    masterWidth.setBounds(topKnobs.removeFromRight(136));

    delaySend.setBounds(bottomKnobs.removeFromLeft(136));
    reverbSend.setBounds(bottomKnobs.removeFromRight(136));
    compMix.setBounds(bottomKnobs.removeFromLeft(136));
    airMix.setBounds(bottomKnobs.removeFromRight(136));

    auto switchArea = cockpit.withSizeKeepingCentre(430, 64);
    globalBypass.setBounds(switchArea.removeFromLeft(82).reduced(4, 12));
    channelEnabled.setBounds(switchArea.removeFromLeft(70).reduced(4, 12));
    compEnabled.setBounds(switchArea.removeFromLeft(64).reduced(4, 12));
    airEnabled.setBounds(switchArea.removeFromLeft(58).reduced(4, 12));
    delayEnabled.setBounds(switchArea.removeFromLeft(70).reduced(4, 12));
    reverbEnabled.setBounds(switchArea.removeFromLeft(78).reduced(4, 12));
}

void F1Dashboard::layoutChannelPage(juce::Rectangle<int> cockpit)
{
    auto controls = cockpit.reduced(28, 0);
    controls.removeFromTop(86);
    controls.removeFromBottom(18);

    constexpr auto knobWidth = 118;
    constexpr auto knobHeight = 110;
    constexpr auto buttonWidth = 118;
    constexpr auto buttonHeight = 46;
    constexpr auto gapX = 11;
    constexpr auto gapY = 8;

    auto layoutKnobs = [] (juce::Rectangle<int> rowArea, std::initializer_list<GordoKnob*> knobs)
    {
        constexpr auto localKnobWidth = knobWidth;
        constexpr auto localKnobHeight = knobHeight;
        constexpr auto localGapX = gapX;

        const auto rowWidth = static_cast<int>(knobs.size()) * localKnobWidth
                            + (static_cast<int>(knobs.size()) - 1) * localGapX;
        auto row = juce::Rectangle<int>(rowWidth, localKnobHeight).withCentre(rowArea.getCentre());

        for (auto* knob : knobs)
        {
            knob->setBounds(row.removeFromLeft(localKnobWidth));
            row.removeFromLeft(localGapX);
        }
    };

    auto topRow = controls.removeFromTop(knobHeight);
    const auto topRowWidth = 5 * knobWidth + buttonWidth + 5 * gapX;
    auto top = juce::Rectangle<int>(topRowWidth, knobHeight).withCentre(topRow.getCentre());

    for (auto* knob : { &channelInputTrim, &channelHighPass, &channelLowPass, &channelDrive, &channelPageMix })
    {
        knob->setBounds(top.removeFromLeft(knobWidth));
        top.removeFromLeft(gapX);
    }

    auto phaseCell = top.removeFromLeft(buttonWidth);
    channelPhaseInvert.setBounds(phaseCell.withSizeKeepingCentre(buttonWidth, buttonHeight));

    controls.removeFromTop(gapY);
    layoutKnobs(controls.removeFromTop(knobHeight),
                { &channelLowGain, &channelLowFreq, &channelLowMidGain, &channelLowMidFreq });

    controls.removeFromTop(gapY);
    layoutKnobs(controls.removeFromTop(knobHeight),
                { &channelHighMidGain, &channelHighMidFreq, &channelHighGain, &channelHighFreq });
}
