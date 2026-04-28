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

    for (auto* knob : { &compInput, &compThreshold, &compOutput, &compAttack, &compRelease, &compRatio, &compPageMix,
                        &compSidechainHp, &compRevision, &compNoise })
        addAndMakeVisible(*knob);

    for (auto* knob : { &airPageAmount, &airMidAir, &airHighAir, &airFrequency, &airPageTone,
                        &airPageDrive, &airDensity, &airDynamic, &airDeEss, &airPageMix, &airPageOutput })
        addAndMakeVisible(*knob);

    for (auto* knob : { &delayTime, &delayDivision, &delayMode, &delayFeedback, &delayPageSend,
                        &delayLeftTime, &delayRightTime, &delayHighPass, &delayLowPass, &delayWidth,
                        &delayLoFi, &delayModDepth, &delayModRate, &delayDucking })
        addAndMakeVisible(*knob);

    for (auto* knob : { &reverbPageMix, &reverbPredelay, &reverbDecay, &reverbSize, &reverbAttack, &reverbWidth,
                        &reverbEarly, &reverbLate, &reverbDiffEarly, &reverbDiffLate, &reverbModRate, &reverbModDepth,
                        &reverbLowCut, &reverbHighCut, &reverbLowDamp, &reverbHighDamp, &reverbMode, &reverbColor,
                        &reverbDucking })
        addAndMakeVisible(*knob);

    for (auto* button : { &globalBypass, &channelEnabled, &compEnabled, &airEnabled, &delayEnabled, &reverbEnabled })
        addAndMakeVisible(*button);

    addAndMakeVisible(channelPhaseInvert);
    addAndMakeVisible(compPageEnabled);
    addAndMakeVisible(airPageEnabled);
    addAndMakeVisible(delaySync);
    addAndMakeVisible(delayLink);
    addAndMakeVisible(delayFreeze);
    addAndMakeVisible(delayPageEnabled);
    addAndMakeVisible(reverbFreeze);
    addAndMakeVisible(reverbSyncPredelay);
    addAndMakeVisible(reverbMonoBass);
    addAndMakeVisible(reverbPageEnabled);

    for (auto* tab : tabs)
    {
        addAndMakeVisible(*tab);
        tab->setRadioGroupId(1001, juce::dontSendNotification);
    }

    tabGlobal.onClick = [this] { setPage(Page::global); };
    tabChannel.onClick = [this] { setPage(Page::channel); };
    tabComp.onClick = [this] { setPage(Page::comp); };
    tabAir.onClick = [this] { setPage(Page::air); };
    tabDelay.onClick = [this] { setPage(Page::delay); };
    tabReverb.onClick = [this] { setPage(Page::reverb); };
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
    gainReductionMeter.setReductionMode(true);
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

bool F1Dashboard::isCompressorPage() const noexcept
{
    return activePage == Page::comp;
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

    const auto compactDisplay = activePage == Page::channel || activePage == Page::comp || activePage == Page::air
                             || activePage == Page::delay || activePage == Page::reverb;
    auto display = compactDisplay
                       ? inner.withSizeKeepingCentre(inner.getWidth() * 0.44f, 58.0f).withY(inner.getY() + 10.0f)
                       : inner.withSizeKeepingCentre(inner.getWidth() * 0.42f, inner.getHeight() * 0.34f);
    g.setColour(juce::Colour(0xff050607));
    g.fillRoundedRectangle(display, 8.0f);
    g.setColour(F1Theme::blue().withAlpha(0.55f));
    g.drawRoundedRectangle(display, 8.0f, 1.5f);

    auto title = juce::String("GLOBAL CONTROL");
    auto route = juce::String("INPUT > CHANNEL > FET > AIR > DELAY > REVERB > OUTPUT");

    if (activePage == Page::channel)
    {
        title = "CHANNEL EQ";
        route = "TRIM > HPF > 4-BAND EQ > DRIVE > MIX";
    }
    else if (activePage == Page::comp)
    {
        title = "FET COMP";
        route = "INPUT > THRESHOLD > DETECTOR HPF > FET GAIN > MIX";
    }
    else if (activePage == Page::air)
    {
        title = "AIR EXCITER";
        route = "MID AIR + HIGH AIR > DYNAMIC DE-ESS > MIX";
    }
    else if (activePage == Page::delay)
    {
        title = "DELAY";
        route = "SEND > TIME/SYNC > FILTERED FEEDBACK > RETURN";
    }
    else if (activePage == Page::reverb)
    {
        title = "REVERB";
        route = "PREDELAY > EARLY > LATE TANK > COLOR > MIX";
    }

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
    else if (activePage == Page::comp)
        layoutCompPage(cockpit);
    else if (activePage == Page::air)
        layoutAirPage(cockpit);
    else if (activePage == Page::delay)
        layoutDelayPage(cockpit);
    else if (activePage == Page::reverb)
        layoutReverbPage(cockpit);
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
    tabComp.setToggleState(activePage == Page::comp, juce::dontSendNotification);
    tabAir.setToggleState(activePage == Page::air, juce::dontSendNotification);
    tabDelay.setToggleState(activePage == Page::delay, juce::dontSendNotification);
    tabReverb.setToggleState(activePage == Page::reverb, juce::dontSendNotification);
    updateControlVisibility();
    resized();
    repaint();
}

void F1Dashboard::updateControlVisibility()
{
    const auto onGlobal = activePage == Page::global;
    const auto onChannel = activePage == Page::channel;
    const auto onComp = activePage == Page::comp;
    const auto onAir = activePage == Page::air;
    const auto onDelay = activePage == Page::delay;
    const auto onReverb = activePage == Page::reverb;

    for (auto* knob : { &inputGain, &outputGain, &channelMix, &compMix, &airMix, &delaySend, &reverbSend, &masterWidth })
        knob->setVisible(onGlobal);

    for (auto* knob : { &channelInputTrim, &channelHighPass, &channelLowPass, &channelLowGain, &channelLowFreq,
                        &channelLowMidGain, &channelLowMidFreq, &channelHighMidGain, &channelHighMidFreq,
                        &channelHighGain, &channelHighFreq, &channelDrive, &channelPageMix })
        knob->setVisible(onChannel);

    for (auto* knob : { &compInput, &compThreshold, &compOutput, &compAttack, &compRelease, &compRatio, &compPageMix,
                        &compSidechainHp, &compRevision, &compNoise })
        knob->setVisible(onComp);

    for (auto* knob : { &airPageAmount, &airMidAir, &airHighAir, &airFrequency, &airPageTone,
                        &airPageDrive, &airDensity, &airDynamic, &airDeEss, &airPageMix, &airPageOutput })
        knob->setVisible(onAir);

    for (auto* knob : { &delayTime, &delayDivision, &delayMode, &delayFeedback, &delayPageSend,
                        &delayLeftTime, &delayRightTime, &delayHighPass, &delayLowPass, &delayWidth,
                        &delayLoFi, &delayModDepth, &delayModRate, &delayDucking })
        knob->setVisible(onDelay);

    for (auto* knob : { &reverbPageMix, &reverbPredelay, &reverbDecay, &reverbSize, &reverbAttack, &reverbWidth,
                        &reverbEarly, &reverbLate, &reverbDiffEarly, &reverbDiffLate, &reverbModRate, &reverbModDepth,
                        &reverbLowCut, &reverbHighCut, &reverbLowDamp, &reverbHighDamp, &reverbMode, &reverbColor,
                        &reverbDucking })
        knob->setVisible(onReverb);

    globalBypass.setVisible(onGlobal);
    channelEnabled.setVisible(onGlobal);
    compEnabled.setVisible(onGlobal);
    airEnabled.setVisible(onGlobal);
    delayEnabled.setVisible(onGlobal);
    reverbEnabled.setVisible(onGlobal);
    channelPhaseInvert.setVisible(onChannel);
    compPageEnabled.setVisible(onComp);
    airPageEnabled.setVisible(onAir);
    delaySync.setVisible(onDelay);
    delayLink.setVisible(onDelay);
    delayFreeze.setVisible(onDelay);
    delayPageEnabled.setVisible(onDelay);
    reverbFreeze.setVisible(onReverb);
    reverbSyncPredelay.setVisible(onReverb);
    reverbMonoBass.setVisible(onReverb);
    reverbPageEnabled.setVisible(onReverb);
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

void F1Dashboard::layoutCompPage(juce::Rectangle<int> cockpit)
{
    auto controls = cockpit.reduced(34, 0);
    controls.removeFromTop(96);
    controls.removeFromBottom(42);

    constexpr auto knobWidth = 118;
    constexpr auto knobHeight = 112;
    constexpr auto buttonWidth = 116;
    constexpr auto buttonHeight = 48;
    constexpr auto gapX = 10;
    constexpr auto gapY = 18;

    auto topRow = controls.removeFromTop(knobHeight);
    const auto topRowWidth = 6 * knobWidth + 5 * gapX;
    auto top = juce::Rectangle<int>(topRowWidth, knobHeight).withCentre(topRow.getCentre());

    for (auto* knob : { &compInput, &compThreshold, &compOutput, &compAttack, &compRelease, &compRatio })
    {
        knob->setBounds(top.removeFromLeft(knobWidth));
        top.removeFromLeft(gapX);
    }

    controls.removeFromTop(gapY);

    auto bottomRow = controls.removeFromTop(knobHeight);
    const auto bottomRowWidth = 4 * knobWidth + buttonWidth + 4 * gapX;
    auto bottom = juce::Rectangle<int>(bottomRowWidth, knobHeight).withCentre(bottomRow.getCentre());

    for (auto* knob : { &compPageMix, &compSidechainHp, &compRevision, &compNoise })
    {
        knob->setBounds(bottom.removeFromLeft(knobWidth));
        bottom.removeFromLeft(gapX);
    }

    compPageEnabled.setBounds(bottom.removeFromLeft(buttonWidth).withSizeKeepingCentre(buttonWidth, buttonHeight));
}

void F1Dashboard::layoutAirPage(juce::Rectangle<int> cockpit)
{
    auto controls = cockpit.reduced(20, 0);
    controls.removeFromTop(96);
    controls.removeFromBottom(44);

    constexpr auto topKnobWidth = 118;
    constexpr auto bottomKnobWidth = 104;
    constexpr auto knobHeight = 112;
    constexpr auto buttonWidth = 96;
    constexpr auto buttonHeight = 48;
    constexpr auto topGapX = 10;
    constexpr auto bottomGapX = 8;
    constexpr auto gapY = 14;

    auto topRow = controls.removeFromTop(knobHeight);
    const auto topRowWidth = 5 * topKnobWidth + 4 * topGapX;
    auto top = juce::Rectangle<int>(topRowWidth, knobHeight).withCentre(topRow.getCentre());

    for (auto* knob : { &airPageAmount, &airMidAir, &airHighAir, &airFrequency, &airPageTone })
    {
        knob->setBounds(top.removeFromLeft(topKnobWidth));
        top.removeFromLeft(topGapX);
    }

    controls.removeFromTop(gapY);

    auto bottomRow = controls.removeFromTop(knobHeight);
    const auto bottomRowWidth = 6 * bottomKnobWidth + buttonWidth + 6 * bottomGapX;
    auto bottom = juce::Rectangle<int>(bottomRowWidth, knobHeight).withCentre(bottomRow.getCentre());

    for (auto* knob : { &airPageDrive, &airDensity, &airDynamic, &airDeEss, &airPageMix, &airPageOutput })
    {
        knob->setBounds(bottom.removeFromLeft(bottomKnobWidth));
        bottom.removeFromLeft(bottomGapX);
    }

    airPageEnabled.setBounds(bottom.removeFromLeft(buttonWidth).withSizeKeepingCentre(buttonWidth, buttonHeight));
}

void F1Dashboard::layoutDelayPage(juce::Rectangle<int> cockpit)
{
    auto controls = cockpit.reduced(14, 0);
    controls.removeFromTop(84);
    controls.removeFromBottom(18);

    constexpr auto knobWidth = 102;
    constexpr auto knobHeight = 96;
    constexpr auto buttonWidth = 84;
    constexpr auto buttonHeight = 44;
    constexpr auto gapX = 8;
    constexpr auto gapY = 8;

    auto makeRow = [=] (juce::Rectangle<int> rowArea, int knobCount, int buttonCount)
    {
        const auto itemCount = knobCount + buttonCount;
        const auto rowWidth = knobCount * knobWidth + buttonCount * buttonWidth + (itemCount - 1) * gapX;
        return juce::Rectangle<int>(rowWidth, knobHeight).withCentre(rowArea.getCentre());
    };

    auto placeKnob = [=] (juce::Rectangle<int>& row, GordoKnob& knob)
    {
        knob.setBounds(row.removeFromLeft(knobWidth));
        row.removeFromLeft(gapX);
    };

    auto placeButton = [=] (juce::Rectangle<int>& row, GordoButton& button)
    {
        button.setBounds(row.removeFromLeft(buttonWidth).withSizeKeepingCentre(buttonWidth, buttonHeight));
        row.removeFromLeft(gapX);
    };

    auto row1 = makeRow(controls.removeFromTop(knobHeight), 5, 1);
    placeKnob(row1, delayTime);
    placeButton(row1, delaySync);
    placeKnob(row1, delayDivision);
    placeKnob(row1, delayMode);
    placeKnob(row1, delayFeedback);
    placeKnob(row1, delayPageSend);

    controls.removeFromTop(gapY);
    auto row2 = makeRow(controls.removeFromTop(knobHeight), 5, 1);
    placeKnob(row2, delayLeftTime);
    placeKnob(row2, delayRightTime);
    placeButton(row2, delayLink);
    placeKnob(row2, delayHighPass);
    placeKnob(row2, delayLowPass);
    placeKnob(row2, delayWidth);

    controls.removeFromTop(gapY);
    auto row3 = makeRow(controls.removeFromTop(knobHeight), 4, 2);
    placeKnob(row3, delayLoFi);
    placeKnob(row3, delayModDepth);
    placeKnob(row3, delayModRate);
    placeKnob(row3, delayDucking);
    placeButton(row3, delayFreeze);
    placeButton(row3, delayPageEnabled);
}

void F1Dashboard::layoutReverbPage(juce::Rectangle<int> cockpit)
{
    auto controls = cockpit.reduced(10, 0);
    controls.removeFromTop(66);

    constexpr auto knobWidth = 98;
    constexpr auto knobHeight = 90;
    constexpr auto buttonWidth = 96;
    constexpr auto buttonHeight = 42;
    constexpr auto gapX = 8;
    constexpr auto gapY = 5;

    auto makeRow = [=] (juce::Rectangle<int> rowArea, int knobCount, int buttonCount)
    {
        const auto itemCount = knobCount + buttonCount;
        const auto rowWidth = knobCount * knobWidth + buttonCount * buttonWidth + (itemCount - 1) * gapX;
        return juce::Rectangle<int>(rowWidth, knobHeight).withCentre(rowArea.getCentre());
    };

    auto placeKnob = [=] (juce::Rectangle<int>& row, GordoKnob& knob)
    {
        knob.setBounds(row.removeFromLeft(knobWidth));
        row.removeFromLeft(gapX);
    };

    auto placeButton = [=] (juce::Rectangle<int>& row, GordoButton& button)
    {
        button.setBounds(row.removeFromLeft(buttonWidth).withSizeKeepingCentre(buttonWidth, buttonHeight));
        row.removeFromLeft(gapX);
    };

    auto row1 = makeRow(controls.removeFromTop(knobHeight), 6, 0);
    placeKnob(row1, reverbPageMix);
    placeKnob(row1, reverbPredelay);
    placeKnob(row1, reverbDecay);
    placeKnob(row1, reverbSize);
    placeKnob(row1, reverbAttack);
    placeKnob(row1, reverbWidth);

    controls.removeFromTop(gapY);
    auto row2 = makeRow(controls.removeFromTop(knobHeight), 6, 0);
    placeKnob(row2, reverbEarly);
    placeKnob(row2, reverbLate);
    placeKnob(row2, reverbDiffEarly);
    placeKnob(row2, reverbDiffLate);
    placeKnob(row2, reverbModRate);
    placeKnob(row2, reverbModDepth);

    controls.removeFromTop(gapY);
    auto row3 = makeRow(controls.removeFromTop(knobHeight), 6, 0);
    placeKnob(row3, reverbLowCut);
    placeKnob(row3, reverbHighCut);
    placeKnob(row3, reverbLowDamp);
    placeKnob(row3, reverbHighDamp);
    placeKnob(row3, reverbMode);
    placeKnob(row3, reverbColor);

    controls.removeFromTop(gapY);
    auto row4 = makeRow(controls.removeFromTop(knobHeight), 1, 4);
    placeKnob(row4, reverbDucking);
    placeButton(row4, reverbFreeze);
    placeButton(row4, reverbSyncPredelay);
    placeButton(row4, reverbMonoBass);
    placeButton(row4, reverbPageEnabled);
}
