#include "F1Dashboard.h"
#include "F1Theme.h"

#include <cmath>

namespace
{
    void paintScrew(juce::Graphics& g, juce::Point<float> centre, float radius)
    {
        auto area = juce::Rectangle<float>(radius * 2.0f, radius * 2.0f).withCentre(centre);
        g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.35f), centre.x, area.getY(),
                                               F1Theme::metal().darker(0.65f), centre.x, area.getBottom(), false));
        g.fillEllipse(area);
        g.setColour(juce::Colour(0xaa000000));
        g.drawEllipse(area, 1.0f);
        g.drawLine(centre.x - radius * 0.52f, centre.y, centre.x + radius * 0.52f, centre.y, 1.4f);
    }

    void paintGripTexture(juce::Graphics& g, juce::Path grip)
    {
        g.setColour(juce::Colours::black.withAlpha(0.42f));
        g.fillPath(grip, juce::AffineTransform::translation(0.0f, 6.0f));
        g.setColour(juce::Colour(0xff050607));
        g.fillPath(grip);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff1a1d22), 0.0f, grip.getBounds().getY(),
                                               juce::Colour(0xff050607), 0.0f, grip.getBounds().getBottom(), false));
        g.fillPath(grip);
        g.setColour(juce::Colour(0x22ffffff));

        const auto bounds = grip.getBounds();
        for (auto y = bounds.getY() + 22.0f; y < bounds.getBottom() - 18.0f; y += 18.0f)
            g.drawLine(bounds.getX() + 20.0f, y, bounds.getRight() - 20.0f, y + 10.0f, 1.2f);

        g.setColour(F1Theme::red().withAlpha(0.50f));
        g.strokePath(grip, juce::PathStrokeType(2.0f));
    }

    void paintBevelPanel(juce::Graphics& g, juce::Path panel, juce::Colour accent)
    {
        g.setColour(juce::Colours::black.withAlpha(0.40f));
        g.fillPath(panel, juce::AffineTransform::translation(0.0f, 5.0f));
        g.setColour(juce::Colour(0xff050607));
        g.fillPath(panel);

        const auto bounds = panel.getBounds();
        g.setGradientFill(juce::ColourGradient(F1Theme::panel().brighter(0.20f), bounds.getCentreX(), bounds.getY(),
                                               F1Theme::panel().darker(0.78f), bounds.getCentreX(), bounds.getBottom(), false));
        g.fillPath(panel);
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.strokePath(panel, juce::PathStrokeType(3.0f));
        g.setColour(accent.withAlpha(0.55f));
        g.strokePath(panel, juce::PathStrokeType(1.4f));
    }

    void paintRoundCap(juce::Graphics& g, juce::Point<float> centre, float radius, juce::Colour accent, const juce::String& text)
    {
        auto area = juce::Rectangle<float>(radius * 2.0f, radius * 2.0f).withCentre(centre);
        g.setColour(juce::Colours::black.withAlpha(0.42f));
        g.fillEllipse(area.expanded(5.0f));
        g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.28f), centre.x, area.getY(),
                                               juce::Colour(0xff050607), centre.x, area.getBottom(), false));
        g.fillEllipse(area);
        g.setColour(accent.withAlpha(0.78f));
        g.drawEllipse(area.reduced(2.0f), 3.0f);
        g.setColour(accent.withAlpha(0.20f));
        g.fillEllipse(area.reduced(radius * 0.34f));
        g.setColour(F1Theme::text());
        g.setFont(juce::FontOptions(radius * 0.48f, juce::Font::bold));
        g.drawFittedText(text, area.toNearestInt().reduced(5), juce::Justification::centred, 1);
    }

    void paintToggleDecor(juce::Graphics& g, juce::Rectangle<float> area, juce::Colour accent, const juce::String& label)
    {
        g.setColour(juce::Colours::black.withAlpha(0.50f));
        g.fillRoundedRectangle(area, 7.0f);
        g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.12f), area.getX(), area.getY(),
                                               F1Theme::metal().darker(0.70f), area.getX(), area.getBottom(), false));
        g.fillRoundedRectangle(area.reduced(1.0f), 6.0f);

        const auto stem = area.withSizeKeepingCentre(7.0f, area.getHeight() * 0.48f).translated(0.0f, -2.0f);
        g.setColour(accent.withAlpha(0.68f));
        g.fillRoundedRectangle(stem, 3.0f);
        g.setColour(F1Theme::text());
        g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
        g.drawFittedText(label, area.toNearestInt().withTrimmedTop(juce::roundToInt(area.getHeight() * 0.55f)).reduced(2, 0),
                         juce::Justification::centred, 1);
    }

    juce::Path makeLeftGrip(juce::Rectangle<float> wheel)
    {
        juce::Path path;
        const auto left = wheel.getX() + 2.0f;
        const auto top = wheel.getY() + wheel.getHeight() * 0.04f;
        const auto bottom = wheel.getBottom() - wheel.getHeight() * 0.04f;
        const auto inner = wheel.getX() + wheel.getWidth() * 0.235f;
        const auto outer = wheel.getX() + wheel.getWidth() * 0.018f;

        path.startNewSubPath(inner, top + 22.0f);
        path.cubicTo(outer + 34.0f, top - 26.0f, left - 8.0f, wheel.getCentreY() - 130.0f, left + 8.0f, wheel.getCentreY());
        path.cubicTo(left - 8.0f, wheel.getCentreY() + 132.0f, outer + 34.0f, bottom + 26.0f, inner, bottom - 34.0f);
        path.cubicTo(inner + 66.0f, bottom - 98.0f, inner + 66.0f, top + 92.0f, inner, top + 22.0f);
        path.closeSubPath();
        return path;
    }

    juce::Path makeRightGrip(juce::Rectangle<float> wheel)
    {
        auto path = makeLeftGrip(wheel);
        path.applyTransform(juce::AffineTransform::scale(-1.0f, 1.0f, wheel.getCentreX(), wheel.getCentreY()));
        return path;
    }
}

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

    // These controls are designed to be host-automation/controller-mapping friendly.
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
    moduleLeds[4].setLedColour(F1Theme::cyan());

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
    auto wheel = bounds.reduced(20, 18).toFloat();
    wheel.removeFromTop(56.0f);

    auto leftGrip = makeLeftGrip(wheel);
    auto rightGrip = makeRightGrip(wheel);
    paintGripTexture(g, leftGrip);
    paintGripTexture(g, rightGrip);

    auto topBridge = wheel.reduced(wheel.getWidth() * 0.175f, 0.0f).removeFromTop(82.0f).translated(0.0f, 6.0f);
    juce::Path bridge;
    bridge.startNewSubPath(topBridge.getX() + 28.0f, topBridge.getY() + 8.0f);
    bridge.lineTo(topBridge.getRight() - 28.0f, topBridge.getY() + 8.0f);
    bridge.lineTo(topBridge.getRight() - 6.0f, topBridge.getCentreY());
    bridge.lineTo(topBridge.getRight() - 46.0f, topBridge.getBottom() - 7.0f);
    bridge.lineTo(topBridge.getX() + 46.0f, topBridge.getBottom() - 7.0f);
    bridge.lineTo(topBridge.getX() + 6.0f, topBridge.getCentreY());
    bridge.closeSubPath();
    paintBevelPanel(g, bridge, F1Theme::red());

    g.setColour(F1Theme::text());
    g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    g.drawFittedText("COCKPIT FX BUS", topBridge.toNearestInt().reduced(90, 20), juce::Justification::centred, 1);
    g.setColour(F1Theme::mutedText());
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawFittedText("HOST MAP READY", topBridge.toNearestInt().reduced(16, 11), juce::Justification::centredLeft, 1);
    g.drawFittedText("HQ AUDIO", topBridge.toNearestInt().reduced(16, 11), juce::Justification::centredRight, 1);

    paintRoundCap(g, topBridge.getTopLeft() + juce::Point<float>(48.0f, 32.0f), 22.0f, F1Theme::green(), "CH");
    paintRoundCap(g, topBridge.getTopRight() + juce::Point<float>(-48.0f, 32.0f), 22.0f, F1Theme::amber(), "FX");

    auto cockpit = wheel.reduced(wheel.getWidth() * 0.145f, wheel.getHeight() * 0.035f);
    cockpit.removeFromTop(52.0f);
    cockpit.removeFromRight(126.0f);

    juce::Path body;
    body.startNewSubPath(cockpit.getX() + 78.0f, cockpit.getY() + 6.0f);
    body.lineTo(cockpit.getRight() - 78.0f, cockpit.getY() + 6.0f);
    body.cubicTo(cockpit.getRight() + 26.0f, cockpit.getY() + 58.0f,
                 cockpit.getRight() + 10.0f, cockpit.getBottom() - 64.0f,
                 cockpit.getRight() - 54.0f, cockpit.getBottom() - 8.0f);
    body.lineTo(cockpit.getX() + 54.0f, cockpit.getBottom() - 8.0f);
    body.cubicTo(cockpit.getX() - 10.0f, cockpit.getBottom() - 64.0f,
                 cockpit.getX() - 26.0f, cockpit.getY() + 58.0f,
                 cockpit.getX() + 78.0f, cockpit.getY() + 6.0f);
    body.closeSubPath();
    paintBevelPanel(g, body, F1Theme::cyan());

    auto inner = cockpit.reduced(15.0f);
    inner.removeFromBottom(4.0f);

    for (auto x = inner.getX() + 18.0f; x < inner.getRight() - 18.0f; x += 34.0f)
    {
        g.setColour(juce::Colour(0x10ffffff));
        g.drawVerticalLine(juce::roundToInt(x), inner.getY() + 10.0f, inner.getBottom() - 10.0f);
    }

    const auto compactDisplay = activePage == Page::channel || activePage == Page::comp || activePage == Page::air
                             || activePage == Page::delay || activePage == Page::reverb;

    auto hub = inner.withSizeKeepingCentre(juce::jmin(310.0f, inner.getHeight() * 0.62f),
                                           juce::jmin(310.0f, inner.getHeight() * 0.62f));
    hub = hub.translated(0.0f, compactDisplay ? -8.0f : -2.0f);

    g.setColour(juce::Colours::black.withAlpha(0.45f));
    g.fillEllipse(hub.expanded(18.0f));
    g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.28f), hub.getCentreX(), hub.getY(),
                                           juce::Colour(0xff050607), hub.getCentreX(), hub.getBottom(), false));
    g.fillEllipse(hub.expanded(7.0f));

    const auto ringRadius = hub.getWidth() * 0.5f + 13.0f;
    for (auto segment = 0; segment < 24; ++segment)
    {
        const auto angle = juce::MathConstants<float>::twoPi * static_cast<float>(segment) / 24.0f;
        const auto p1 = juce::Point<float>(hub.getCentreX() + std::cos(angle) * ringRadius,
                                           hub.getCentreY() + std::sin(angle) * ringRadius);
        const auto p2 = juce::Point<float>(hub.getCentreX() + std::cos(angle) * (ringRadius - 12.0f),
                                           hub.getCentreY() + std::sin(angle) * (ringRadius - 12.0f));
        g.setColour((segment % 6 == 0 ? F1Theme::red()
                                      : segment % 3 == 0 ? F1Theme::amber()
                                                         : F1Theme::cyan()).withAlpha(segment % 3 == 0 ? 0.72f : 0.30f));
        g.drawLine(p1.x, p1.y, p2.x, p2.y, segment % 6 == 0 ? 2.4f : 1.2f);
    }

    g.setColour(juce::Colour(0xff050607));
    g.fillEllipse(hub.reduced(26.0f));
    g.setGradientFill(juce::ColourGradient(F1Theme::panel().brighter(0.18f), hub.getCentreX(), hub.getY(),
                                           F1Theme::panel().darker(0.85f), hub.getCentreX(), hub.getBottom(), false));
    g.fillEllipse(hub.reduced(32.0f));

    auto display = compactDisplay
                       ? hub.withSizeKeepingCentre(hub.getWidth() * 0.88f, 72.0f).withY(hub.getY() + 34.0f)
                       : hub.reduced(54.0f);
    g.setColour(juce::Colour(0xff050607));
    if (compactDisplay)
        g.fillRoundedRectangle(display, 12.0f);
    else
        g.fillEllipse(display);

    g.setGradientFill(juce::ColourGradient(F1Theme::blue().withAlpha(0.26f), display.getCentreX(), display.getY(),
                                           juce::Colours::transparentBlack, display.getCentreX(), display.getBottom(), false));
    if (compactDisplay)
        g.fillRoundedRectangle(display.reduced(3.0f), 8.0f);
    else
        g.fillEllipse(display.reduced(5.0f));

    g.setColour(F1Theme::blue().withAlpha(0.70f));
    if (compactDisplay)
        g.drawRoundedRectangle(display, 12.0f, 1.5f);
    else
        g.drawEllipse(display, 2.0f);

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

    auto ledRing = hub.expanded(12.0f);
    for (auto i = 0; i < 16; ++i)
    {
        const auto angle = juce::MathConstants<float>::twoPi * static_cast<float>(i) / 16.0f - juce::MathConstants<float>::halfPi;
        const auto dot = juce::Point<float>(hub.getCentreX() + std::cos(angle) * ledRing.getWidth() * 0.5f,
                                            hub.getCentreY() + std::sin(angle) * ledRing.getHeight() * 0.5f);
        g.setColour((i % 5 == 0 ? F1Theme::cyan() : F1Theme::mutedText()).withAlpha(i % 5 == 0 ? 0.65f : 0.20f));
        g.fillEllipse(juce::Rectangle<float>(6.0f, 6.0f).withCentre(dot));
    }

    const auto leftPod = juce::Rectangle<float>(inner.getX() + 22.0f, inner.getY() + 114.0f, 102.0f, inner.getHeight() - 216.0f);
    const auto rightPod = leftPod.withX(inner.getRight() - 124.0f);
    g.setColour(juce::Colour(0xaa050607));
    g.fillRoundedRectangle(leftPod, 24.0f);
    g.fillRoundedRectangle(rightPod, 24.0f);
    g.setColour(F1Theme::mutedText().withAlpha(0.18f));
    g.drawRoundedRectangle(leftPod, 24.0f, 1.2f);
    g.drawRoundedRectangle(rightPod, 24.0f, 1.2f);

    auto switchDeck = juce::Rectangle<float>(hub.getCentreX() - 170.0f, inner.getBottom() - 62.0f, 340.0f, 48.0f);
    g.setColour(juce::Colour(0xaa050607));
    g.fillRoundedRectangle(switchDeck, 10.0f);
    paintToggleDecor(g, switchDeck.removeFromLeft(74.0f).reduced(13.0f, 7.0f), F1Theme::green(), "MAP");
    paintToggleDecor(g, switchDeck.removeFromLeft(74.0f).reduced(13.0f, 7.0f), F1Theme::red(), "CUT");
    paintToggleDecor(g, switchDeck.removeFromLeft(74.0f).reduced(13.0f, 7.0f), F1Theme::amber(), "BUS");
    paintToggleDecor(g, switchDeck.removeFromLeft(74.0f).reduced(13.0f, 7.0f), F1Theme::cyan(), "AUX");

    const auto screwArea = cockpit.reduced(28.0f, 24.0f);
    paintScrew(g, screwArea.getTopLeft() + juce::Point<float>(8.0f, 8.0f), 6.0f);
    paintScrew(g, screwArea.getTopRight() + juce::Point<float>(-8.0f, 8.0f), 6.0f);
    paintScrew(g, screwArea.getBottomLeft() + juce::Point<float>(8.0f, -8.0f), 6.0f);
    paintScrew(g, screwArea.getBottomRight() + juce::Point<float>(-8.0f, -8.0f), 6.0f);

    g.setColour(F1Theme::mutedText().withAlpha(0.55f));
    g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    g.drawFittedText("UI: F1 Cockpit", getLocalBounds().reduced(34).removeFromBottom(18), juce::Justification::centredRight, 1);
}

void F1Dashboard::resized()
{
    auto area = getLocalBounds().reduced(24);
    auto tabRail = area.removeFromTop(112);
    auto tabArea = tabRail.removeFromBottom(50)
                      .withSizeKeepingCentre(juce::jmin(980, tabRail.getWidth() - 190), 48);
    const auto tabWidth = tabArea.getWidth() / static_cast<int>(tabs.size());

    for (auto index = 0; index < static_cast<int>(tabs.size()); ++index)
    {
        auto cell = tabArea.removeFromLeft(tabWidth).reduced(5, 5);
        const auto edgeDrop = juce::jmax(0, std::abs(index - 3) - 1) * 3;
        tabs[static_cast<size_t>(index)]->setBounds(cell.translated(0, edgeDrop));
    }

    auto meterArea = area.removeFromRight(170).reduced(8, 16);
    inputMeter.setBounds(meterArea.removeFromLeft(48));
    meterArea.removeFromLeft(8);
    outputMeter.setBounds(meterArea.removeFromLeft(48));
    meterArea.removeFromLeft(8);
    gainReductionMeter.setBounds(meterArea.removeFromLeft(48));

    auto cockpit = area.reduced(58, 30);

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

    auto ledArea = cockpit.withSizeKeepingCentre(300, 24).translated(0, activePage == Page::global ? -68 : -76);
    for (auto& led : moduleLeds)
        led.setBounds(ledArea.removeFromLeft(60).withSizeKeepingCentre(18, 18));
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
    auto topKnobs = cockpit.removeFromTop(142);
    auto bottomKnobs = cockpit.removeFromBottom(142);

    inputGain.setBounds(topKnobs.removeFromLeft(134));
    outputGain.setBounds(topKnobs.removeFromRight(134));
    channelMix.setBounds(topKnobs.removeFromLeft(134));
    masterWidth.setBounds(topKnobs.removeFromRight(134));

    delaySend.setBounds(bottomKnobs.removeFromLeft(134));
    reverbSend.setBounds(bottomKnobs.removeFromRight(134));
    compMix.setBounds(bottomKnobs.removeFromLeft(134));
    airMix.setBounds(bottomKnobs.removeFromRight(134));

    auto switchArea = cockpit.withSizeKeepingCentre(558, 56);
    globalBypass.setBounds(switchArea.removeFromLeft(88).reduced(4, 9));
    channelEnabled.setBounds(switchArea.removeFromLeft(86).reduced(4, 9));
    compEnabled.setBounds(switchArea.removeFromLeft(76).reduced(4, 9));
    airEnabled.setBounds(switchArea.removeFromLeft(70).reduced(4, 9));
    delayEnabled.setBounds(switchArea.removeFromLeft(82).reduced(4, 9));
    reverbEnabled.setBounds(switchArea.removeFromLeft(90).reduced(4, 9));
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
