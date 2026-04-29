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
        const auto bounds = grip.getBounds();

        g.setColour(juce::Colours::black.withAlpha(0.50f));
        g.fillPath(grip, juce::AffineTransform::translation(0.0f, 9.0f));
        g.setColour(juce::Colours::black.withAlpha(0.28f));
        g.fillPath(grip, juce::AffineTransform::translation(0.0f, 18.0f));
        g.setColour(juce::Colour(0xff050607));
        g.fillPath(grip);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff252a31), 0.0f, bounds.getY(),
                                               juce::Colour(0xff020304), 0.0f, bounds.getBottom(), false));
        g.fillPath(grip);

        g.setColour(juce::Colour(0x30ffffff));
        g.drawLine(bounds.getX() + bounds.getWidth() * 0.32f, bounds.getY() + 18.0f,
                   bounds.getX() + bounds.getWidth() * 0.58f, bounds.getBottom() - 26.0f, 2.0f);

        for (auto y = bounds.getY() + 22.0f; y < bounds.getBottom() - 18.0f; y += 18.0f)
        {
            g.setColour(juce::Colour(0x22ffffff));
            g.drawLine(bounds.getX() + 20.0f, y, bounds.getRight() - 20.0f, y + 10.0f, 1.2f);
            g.setColour(juce::Colour(0x44000000));
            g.drawLine(bounds.getX() + 22.0f, y + 5.0f, bounds.getRight() - 22.0f, y + 15.0f, 1.0f);
        }

        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.strokePath(grip, juce::PathStrokeType(5.0f));
        g.setColour(F1Theme::red().withAlpha(0.62f));
        g.strokePath(grip, juce::PathStrokeType(1.6f));
    }

    void paintGripOpening(juce::Graphics& g, juce::Rectangle<float> wheel, bool leftSide)
    {
        const auto x = leftSide ? wheel.getX() + wheel.getWidth() * 0.070f
                                : wheel.getRight() - wheel.getWidth() * 0.225f;
        auto opening = juce::Rectangle<float>(wheel.getWidth() * 0.155f, wheel.getHeight() * 0.595f)
                           .withPosition(x, wheel.getY() + wheel.getHeight() * 0.205f);

        juce::Path cutout;
        const auto round = opening.getWidth() * 0.46f;
        cutout.startNewSubPath(opening.getX() + round, opening.getY());
        cutout.lineTo(opening.getRight() - round * 0.72f, opening.getY());
        cutout.cubicTo(opening.getRight() + 16.0f, opening.getY() + opening.getHeight() * 0.18f,
                       opening.getRight() + 14.0f, opening.getY() + opening.getHeight() * 0.38f,
                       opening.getRight() - 14.0f, opening.getCentreY());
        cutout.cubicTo(opening.getRight() + 14.0f, opening.getY() + opening.getHeight() * 0.62f,
                       opening.getRight() + 12.0f, opening.getBottom() - opening.getHeight() * 0.14f,
                       opening.getRight() - round, opening.getBottom());
        cutout.lineTo(opening.getX() + round, opening.getBottom());
        cutout.cubicTo(opening.getX() - 18.0f, opening.getBottom() - opening.getHeight() * 0.20f,
                       opening.getX() - 16.0f, opening.getY() + opening.getHeight() * 0.20f,
                       opening.getX() + round, opening.getY());
        cutout.closeSubPath();

        g.setColour(juce::Colour(0xff020304));
        g.fillPath(cutout);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0x66000000), opening.getCentreX(), opening.getY(),
                                               juce::Colour(0x22000000), opening.getCentreX(), opening.getBottom(), false));
        g.fillPath(cutout);
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.strokePath(cutout, juce::PathStrokeType(3.0f));
        g.setColour(juce::Colours::black.withAlpha(0.72f));
        g.strokePath(cutout, juce::PathStrokeType(7.0f));

        const auto bridge = leftSide ? opening.withTrimmedRight(opening.getWidth() * 0.56f)
                                     : opening.withTrimmedLeft(opening.getWidth() * 0.56f);
        g.setColour(juce::Colour(0xff111419));
        g.fillRoundedRectangle(bridge.expanded(8.0f, 20.0f), 22.0f);
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

    void paintCarbonWeave(juce::Graphics& g, juce::Rectangle<float> area)
    {
        g.setColour(juce::Colour(0x12000000));
        for (auto x = area.getX() - area.getHeight(); x < area.getRight(); x += 14.0f)
            g.drawLine(x, area.getBottom(), x + area.getHeight(), area.getY(), 1.0f);

        g.setColour(juce::Colour(0x10ffffff));
        for (auto x = area.getX(); x < area.getRight() + area.getHeight(); x += 18.0f)
            g.drawLine(x, area.getY(), x - area.getHeight(), area.getBottom(), 1.0f);

        g.setColour(juce::Colour(0x0f9aa3ad));
        for (auto y = area.getY() + 8.0f; y < area.getBottom(); y += 22.0f)
            g.drawHorizontalLine(juce::roundToInt(y), area.getX() + 10.0f, area.getRight() - 10.0f);
    }

    void paintRectHardwarePanel(juce::Graphics& g, juce::Rectangle<float> area, juce::Colour accent, float corner = 14.0f)
    {
        g.setColour(juce::Colours::black.withAlpha(0.50f));
        g.fillRoundedRectangle(area.translated(0.0f, 5.0f), corner);
        g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.10f), area.getCentreX(), area.getY(),
                                               juce::Colour(0xff050607), area.getCentreX(), area.getBottom(), false));
        g.fillRoundedRectangle(area, corner);
        g.setColour(accent.withAlpha(0.38f));
        g.drawRoundedRectangle(area.reduced(1.0f), corner - 1.0f, 1.2f);
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawHorizontalLine(juce::roundToInt(area.getY() + 3.0f), area.getX() + corner, area.getRight() - corner);
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

    void paintHubRouteLabel(juce::Graphics& g, juce::Point<float> centre, float radius, float angle,
                            juce::Colour colour, const juce::String& text)
    {
        const auto point = juce::Point<float>(centre.x + std::cos(angle) * radius,
                                              centre.y + std::sin(angle) * radius);
        auto area = juce::Rectangle<float>(38.0f, 20.0f).withCentre(point);

        g.setColour(juce::Colours::black.withAlpha(0.54f));
        g.fillRoundedRectangle(area.expanded(2.0f), 7.0f);
        g.setColour(colour.withAlpha(0.82f));
        g.drawRoundedRectangle(area, 6.0f, 1.2f);
        g.setColour(F1Theme::text());
        g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
        g.drawFittedText(text, area.toNearestInt().reduced(2, 0), juce::Justification::centred, 1);
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
        const auto top = wheel.getY() + wheel.getHeight() * 0.04f;
        const auto bottom = wheel.getBottom() - wheel.getHeight() * 0.04f;
        const auto inner = wheel.getX() + wheel.getWidth() * 0.315f;
        const auto outer = wheel.getX() + wheel.getWidth() * 0.004f;

        path.startNewSubPath(inner, top + 30.0f);
        path.cubicTo(outer + 86.0f, top - 58.0f, outer - 22.0f, wheel.getCentreY() - 202.0f, outer + 22.0f, wheel.getCentreY() - 92.0f);
        path.cubicTo(outer - 12.0f, wheel.getCentreY() - 26.0f, outer - 12.0f, wheel.getCentreY() + 26.0f, outer + 22.0f, wheel.getCentreY() + 92.0f);
        path.cubicTo(outer - 22.0f, wheel.getCentreY() + 204.0f, outer + 88.0f, bottom + 56.0f, inner, bottom - 36.0f);
        path.cubicTo(inner + 88.0f, bottom - 118.0f, inner + 88.0f, top + 104.0f, inner, top + 30.0f);
        path.closeSubPath();
        return path;
    }

    juce::Path makeRightGrip(juce::Rectangle<float> wheel)
    {
        auto path = makeLeftGrip(wheel);
        path.applyTransform(juce::AffineTransform::scale(-1.0f, 1.0f, wheel.getCentreX(), wheel.getCentreY()));
        return path;
    }

    juce::Path makeWheelBackplate(juce::Rectangle<float> wheel)
    {
        juce::Path path;
        const auto cx = wheel.getCentreX();
        const auto top = wheel.getY() + wheel.getHeight() * 0.045f;
        const auto bottom = wheel.getBottom() - wheel.getHeight() * 0.025f;
        const auto left = wheel.getX() + wheel.getWidth() * 0.075f;
        const auto right = wheel.getRight() - wheel.getWidth() * 0.075f;

        path.startNewSubPath(cx - wheel.getWidth() * 0.30f, top + 14.0f);
        path.cubicTo(cx - wheel.getWidth() * 0.47f, top - 16.0f,
                     left + 24.0f, wheel.getCentreY() - wheel.getHeight() * 0.34f,
                     left + 38.0f, wheel.getCentreY() - wheel.getHeight() * 0.08f);
        path.cubicTo(left + 18.0f, wheel.getCentreY() + wheel.getHeight() * 0.24f,
                     cx - wheel.getWidth() * 0.33f, bottom + 14.0f,
                     cx - wheel.getWidth() * 0.13f, bottom - 16.0f);
        path.cubicTo(cx - wheel.getWidth() * 0.08f, bottom - 4.0f,
                     cx + wheel.getWidth() * 0.08f, bottom - 4.0f,
                     cx + wheel.getWidth() * 0.13f, bottom - 16.0f);
        path.cubicTo(cx + wheel.getWidth() * 0.33f, bottom + 14.0f,
                     right - 18.0f, wheel.getCentreY() + wheel.getHeight() * 0.24f,
                     right - 38.0f, wheel.getCentreY() - wheel.getHeight() * 0.08f);
        path.cubicTo(right - 24.0f, wheel.getCentreY() - wheel.getHeight() * 0.34f,
                     cx + wheel.getWidth() * 0.47f, top - 16.0f,
                     cx + wheel.getWidth() * 0.30f, top + 14.0f);
        path.closeSubPath();
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
    auto wheel = bounds.reduced(8, 8).toFloat();
    wheel.removeFromTop(8.0f);

    auto backplate = makeWheelBackplate(wheel);
    g.setColour(juce::Colours::black.withAlpha(0.68f));
    g.fillPath(backplate, juce::AffineTransform::translation(0.0f, 14.0f));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff151a20), wheel.getCentreX(), wheel.getY() + 22.0f,
                                           juce::Colour(0xff020304), wheel.getCentreX(), wheel.getBottom(), false));
    g.fillPath(backplate);
    paintCarbonWeave(g, backplate.getBounds().reduced(18.0f));
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.strokePath(backplate, juce::PathStrokeType(6.0f));
    g.setColour(F1Theme::cyan().withAlpha(0.26f));
    g.strokePath(backplate, juce::PathStrokeType(1.6f));

    auto leftGrip = makeLeftGrip(wheel);
    auto rightGrip = makeRightGrip(wheel);
    paintGripTexture(g, leftGrip);
    paintGripTexture(g, rightGrip);
    paintGripOpening(g, wheel, true);
    paintGripOpening(g, wheel, false);

    auto topBridge = wheel.reduced(wheel.getWidth() * 0.070f, 0.0f).removeFromTop(106.0f).translated(0.0f, 2.0f);
    juce::Path bridge;
    bridge.startNewSubPath(topBridge.getX() + 56.0f, topBridge.getY() + 7.0f);
    bridge.lineTo(topBridge.getRight() - 56.0f, topBridge.getY() + 7.0f);
    bridge.lineTo(topBridge.getRight() - 8.0f, topBridge.getCentreY() + 3.0f);
    bridge.lineTo(topBridge.getRight() - 72.0f, topBridge.getBottom() - 8.0f);
    bridge.lineTo(topBridge.getX() + 72.0f, topBridge.getBottom() - 8.0f);
    bridge.lineTo(topBridge.getX() + 8.0f, topBridge.getCentreY() + 3.0f);
    bridge.closeSubPath();
    paintBevelPanel(g, bridge, F1Theme::red());

    g.setColour(F1Theme::mutedText().withAlpha(0.34f));
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawFittedText("INTEGRATED CONTROL BRIDGE", topBridge.toNearestInt().reduced(132, 30), juce::Justification::centred, 1);

    for (auto x = topBridge.getX() + 184.0f; x < topBridge.getRight() - 184.0f; x += 18.0f)
    {
        g.setColour(juce::Colours::white.withAlpha(0.07f));
        g.drawVerticalLine(juce::roundToInt(x), topBridge.getY() + 18.0f, topBridge.getBottom() - 18.0f);
    }

    paintRoundCap(g, topBridge.getTopLeft() + juce::Point<float>(58.0f, 40.0f), 25.0f, F1Theme::green(), "IN");
    paintRoundCap(g, topBridge.getTopRight() + juce::Point<float>(-58.0f, 40.0f), 25.0f, F1Theme::amber(), "OUT");

    auto cockpit = wheel.reduced(wheel.getWidth() * 0.135f, wheel.getHeight() * 0.020f);
    cockpit.removeFromTop(78.0f);

    juce::Path body;
    body.startNewSubPath(cockpit.getX() + 116.0f, cockpit.getY() + 2.0f);
    body.lineTo(cockpit.getRight() - 116.0f, cockpit.getY() + 2.0f);
    body.cubicTo(cockpit.getRight() + 50.0f, cockpit.getY() + 54.0f,
                 cockpit.getRight() + 54.0f, cockpit.getCentreY() - 42.0f,
                 cockpit.getRight() - 8.0f, cockpit.getCentreY() + 18.0f);
    body.cubicTo(cockpit.getRight() + 48.0f, cockpit.getBottom() - 92.0f,
                 cockpit.getRight() - 26.0f, cockpit.getBottom() + 28.0f,
                 cockpit.getRight() - 150.0f, cockpit.getBottom() - 4.0f);
    body.lineTo(cockpit.getCentreX() + 118.0f, cockpit.getBottom() + 4.0f);
    body.cubicTo(cockpit.getCentreX() + 60.0f, cockpit.getBottom() + 24.0f,
                 cockpit.getCentreX() - 60.0f, cockpit.getBottom() + 24.0f,
                 cockpit.getCentreX() - 118.0f, cockpit.getBottom() + 4.0f);
    body.lineTo(cockpit.getX() + 150.0f, cockpit.getBottom() - 4.0f);
    body.cubicTo(cockpit.getX() + 26.0f, cockpit.getBottom() + 28.0f,
                 cockpit.getX() - 48.0f, cockpit.getBottom() - 92.0f,
                 cockpit.getX() + 8.0f, cockpit.getCentreY() + 18.0f);
    body.cubicTo(cockpit.getX() - 54.0f, cockpit.getCentreY() - 42.0f,
                 cockpit.getX() - 50.0f, cockpit.getY() + 54.0f,
                 cockpit.getX() + 116.0f, cockpit.getY() + 2.0f);
    body.closeSubPath();
    paintBevelPanel(g, body, F1Theme::cyan());
    paintCarbonWeave(g, cockpit.reduced(34.0f, 20.0f));

    auto shoulderLeft = juce::Rectangle<float>(cockpit.getX() + 4.0f, cockpit.getY() + 48.0f, 178.0f, 98.0f);
    auto shoulderRight = shoulderLeft.withX(cockpit.getRight() - 182.0f);
    g.setColour(juce::Colour(0xff090b0f));
    g.fillRoundedRectangle(shoulderLeft, 34.0f);
    g.fillRoundedRectangle(shoulderRight, 34.0f);
    g.setColour(F1Theme::mutedText().withAlpha(0.24f));
    g.drawRoundedRectangle(shoulderLeft, 34.0f, 1.0f);
    g.drawRoundedRectangle(shoulderRight, 34.0f, 1.0f);
    g.setColour(F1Theme::text().withAlpha(0.62f));
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawFittedText("MACRO", shoulderLeft.toNearestInt().reduced(12), juce::Justification::centred, 1);
    g.drawFittedText("RACK", shoulderRight.toNearestInt().reduced(12), juce::Justification::centred, 1);

    for (auto* tab : tabs)
    {
        const auto tabMount = tab->getBounds().toFloat().expanded(5.0f, 6.0f);
        if (tabMount.getWidth() > 0.0f)
            paintRectHardwarePanel(g, tabMount, tab->getToggleState() ? F1Theme::amber() : F1Theme::mutedText(), 10.0f);
    }

    auto meterHousing = inputMeter.getBounds().toFloat()
                            .getUnion(outputMeter.getBounds().toFloat())
                            .getUnion(gainReductionMeter.getBounds().toFloat())
                            .expanded(15.0f, 18.0f);
    if (meterHousing.getWidth() > 0.0f)
    {
        paintRectHardwarePanel(g, meterHousing, F1Theme::green(), 18.0f);
        g.setColour(F1Theme::text().withAlpha(0.66f));
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.drawFittedText("METER POD",
                         juce::Rectangle<int>(juce::roundToInt(meterHousing.getX()),
                                              juce::roundToInt(meterHousing.getY()),
                                              juce::roundToInt(meterHousing.getWidth()),
                                              18),
                         juce::Justification::centred,
                         1);
    }

    // Decorative cockpit buttons only; parameter-backed controls remain the APVTS-attached components.
    paintRoundCap(g, shoulderLeft.getCentre() + juce::Point<float>(-52.0f, -6.0f), 17.0f, F1Theme::green(), "A");
    paintRoundCap(g, shoulderLeft.getCentre() + juce::Point<float>(50.0f, 16.0f), 17.0f, F1Theme::red(), "B");
    paintRoundCap(g, shoulderRight.getCentre() + juce::Point<float>(-50.0f, 16.0f), 17.0f, F1Theme::amber(), "C");
    paintRoundCap(g, shoulderRight.getCentre() + juce::Point<float>(52.0f, -6.0f), 17.0f, F1Theme::cyan(), "D");

    auto inner = cockpit.reduced(14.0f);
    inner.removeFromBottom(4.0f);

    for (auto x = inner.getX() + 18.0f; x < inner.getRight() - 18.0f; x += 34.0f)
    {
        g.setColour(juce::Colour(0x10ffffff));
        g.drawVerticalLine(juce::roundToInt(x), inner.getY() + 10.0f, inner.getBottom() - 10.0f);
    }

    const auto compactDisplay = activePage == Page::channel || activePage == Page::comp || activePage == Page::air
                             || activePage == Page::delay || activePage == Page::reverb;

    const auto hubSize = juce::jmin(420.0f, inner.getHeight() * 0.86f);
    auto hub = inner.withSizeKeepingCentre(hubSize, hubSize);
    hub = hub.translated(0.0f, compactDisplay ? -18.0f : -8.0f);

    g.setColour(juce::Colours::black.withAlpha(0.45f));
    g.fillEllipse(hub.expanded(24.0f).translated(0.0f, 8.0f));
    g.setGradientFill(juce::ColourGradient(F1Theme::metal().brighter(0.28f), hub.getCentreX(), hub.getY(),
                                           juce::Colour(0xff050607), hub.getCentreX(), hub.getBottom(), false));
    g.fillEllipse(hub.expanded(12.0f));

    const auto ringRadius = hub.getWidth() * 0.5f + 18.0f;
    for (auto segment = 0; segment < 36; ++segment)
    {
        const auto angle = juce::MathConstants<float>::twoPi * static_cast<float>(segment) / 36.0f;
        const auto p1 = juce::Point<float>(hub.getCentreX() + std::cos(angle) * ringRadius,
                                           hub.getCentreY() + std::sin(angle) * ringRadius);
        const auto p2 = juce::Point<float>(hub.getCentreX() + std::cos(angle) * (ringRadius - (segment % 6 == 0 ? 18.0f : 10.0f)),
                                           hub.getCentreY() + std::sin(angle) * (ringRadius - (segment % 6 == 0 ? 18.0f : 10.0f)));
        g.setColour((segment % 9 == 0 ? F1Theme::red()
                                      : segment % 6 == 0 ? F1Theme::amber()
                                                         : F1Theme::cyan()).withAlpha(segment % 6 == 0 ? 0.78f : 0.28f));
        g.drawLine(p1.x, p1.y, p2.x, p2.y, segment % 6 == 0 ? 2.6f : 1.1f);
    }

    g.setColour(juce::Colour(0xff050607));
    g.fillEllipse(hub.reduced(22.0f));
    g.setGradientFill(juce::ColourGradient(F1Theme::panel().brighter(0.18f), hub.getCentreX(), hub.getY(),
                                           F1Theme::panel().darker(0.85f), hub.getCentreX(), hub.getBottom(), false));
    g.fillEllipse(hub.reduced(34.0f));
    g.setColour(juce::Colour(0xff050607));
    g.drawEllipse(hub.reduced(56.0f), 12.0f);

    const auto routeRadius = hub.getWidth() * 0.43f;
    paintHubRouteLabel(g, hub.getCentre(), routeRadius, juce::MathConstants<float>::pi * 1.10f, F1Theme::green(), "IN");
    paintHubRouteLabel(g, hub.getCentre(), routeRadius, juce::MathConstants<float>::pi * 1.30f, F1Theme::green(), "CH");
    paintHubRouteLabel(g, hub.getCentre(), routeRadius, juce::MathConstants<float>::pi * 1.52f, F1Theme::red(), "COMP");
    paintHubRouteLabel(g, hub.getCentre(), routeRadius, juce::MathConstants<float>::pi * 1.74f, F1Theme::blue(), "AIR");
    paintHubRouteLabel(g, hub.getCentre(), routeRadius, juce::MathConstants<float>::pi * 1.96f, F1Theme::amber(), "DLY");
    paintHubRouteLabel(g, hub.getCentre(), routeRadius, juce::MathConstants<float>::pi * 2.18f, F1Theme::cyan(), "REV");
    paintHubRouteLabel(g, hub.getCentre(), routeRadius, juce::MathConstants<float>::pi * 2.40f, F1Theme::amber(), "OUT");

    auto display = compactDisplay
                       ? hub.withSizeKeepingCentre(hub.getWidth() * 0.78f, 76.0f).withY(hub.getY() + 42.0f)
                       : hub.reduced(72.0f);
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
    auto route = juce::String("INPUT > CHANNEL > COMP > AIR > DELAY > REVERB > OUTPUT");

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
    auto frame = getLocalBounds().reduced(24);
    const auto tabCentreX = frame.getCentreX();
    const auto tabCentreY = frame.getY() + 168;
    constexpr auto tabW = 92;
    constexpr auto tabH = 38;

    const std::array<juce::Point<int>, 8> tabCentres {
        juce::Point<int>(tabCentreX - 286, tabCentreY + 18),
        juce::Point<int>(tabCentreX - 202, tabCentreY - 48),
        juce::Point<int>(tabCentreX - 88, tabCentreY - 78),
        juce::Point<int>(tabCentreX + 88, tabCentreY - 78),
        juce::Point<int>(tabCentreX + 202, tabCentreY - 48),
        juce::Point<int>(tabCentreX + 286, tabCentreY + 18),
        juce::Point<int>(tabCentreX - 154, tabCentreY + 94),
        juce::Point<int>(tabCentreX + 154, tabCentreY + 94)
    };

    for (auto index = 0; index < static_cast<int>(tabs.size()); ++index)
        tabs[static_cast<size_t>(index)]->setBounds(juce::Rectangle<int>(tabW, tabH).withCentre(tabCentres[static_cast<size_t>(index)]));

    auto area = frame;
    area.removeFromTop(112);

    auto meterArea = juce::Rectangle<int>(152, area.getHeight() - 68)
                         .withCentre(juce::Point<int>(area.getRight() - 90, area.getCentreY() + 8))
                         .reduced(8, 16);
    inputMeter.setBounds(meterArea.removeFromLeft(48));
    meterArea.removeFromLeft(8);
    outputMeter.setBounds(meterArea.removeFromLeft(48));
    meterArea.removeFromLeft(8);
    gainReductionMeter.setBounds(meterArea.removeFromLeft(48));

    auto cockpit = area.reduced(40, 24);
    cockpit.removeFromRight(96);

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
    auto controls = cockpit.reduced(16, 4);

    constexpr auto knobWidth = 130;
    constexpr auto knobHeight = 126;
    const auto leftX = controls.getX() + 12;
    const auto rightX = controls.getRight() - knobWidth - 12;
    const auto topY = controls.getY() + 4;
    const auto midY = controls.getCentreY() - knobHeight / 2 - 8;
    const auto bottomY = controls.getBottom() - knobHeight - 6;
    const auto centreX = controls.getCentreX();

    // Global macros stay exposed for host automation and future wheel/controller mapping.
    inputGain.setBounds(leftX, topY, knobWidth, knobHeight);
    outputGain.setBounds(rightX, topY, knobWidth, knobHeight);
    channelMix.setBounds(leftX, midY, knobWidth, knobHeight);
    masterWidth.setBounds(rightX, midY, knobWidth, knobHeight);
    delaySend.setBounds(leftX, bottomY, knobWidth, knobHeight);
    reverbSend.setBounds(rightX, bottomY, knobWidth, knobHeight);
    compMix.setBounds(centreX - knobWidth - 42, bottomY, knobWidth, knobHeight);
    airMix.setBounds(centreX + 42, bottomY, knobWidth, knobHeight);

    auto switchArea = juce::Rectangle<int>(568, 58).withCentre(juce::Point<int>(centreX, controls.getCentreY() + 34));
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
