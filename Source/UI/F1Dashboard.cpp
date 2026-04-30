#include "F1Dashboard.h"
#include "F1Theme.h"
#include "BinaryData.h"

#include <cmath>
#include <limits>

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
    wheelImage = juce::ImageCache::getFromMemory(BinaryData::sss33VOLANTE_base_png,
                                                 BinaryData::sss33VOLANTE_base_pngSize);
    loadWheelPointerImages();

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
        tab->setVisible(false);
    }

    tabGlobal.onClick = [this] { setPage(Page::global); };
    tabChannel.onClick = [this] { setPage(Page::channel); };
    tabComp.onClick = [this] { setPage(Page::comp); };
    tabAir.onClick = [this] { setPage(Page::air); };
    tabDelay.onClick = [this] { setPage(Page::delay); };
    tabRouting.onClick = [this] { setPage(Page::routing); };
    tabMeters.onClick = [this] { setPage(Page::meters); };

    moduleLeds[0].setLedColour(F1Theme::green());
    moduleLeds[1].setLedColour(F1Theme::red());
    moduleLeds[2].setLedColour(F1Theme::blue());
    moduleLeds[3].setLedColour(F1Theme::amber());

    for (auto& led : moduleLeds)
        addAndMakeVisible(led);

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    gainReductionMeter.setReductionMode(true);
    addAndMakeVisible(gainReductionMeter);

    setPage(Page::global);
}

void F1Dashboard::setParameterState(juce::AudioProcessorValueTreeState& state) noexcept
{
    parameterState = &state;
}

void F1Dashboard::setMeterLevels(float input, float output, float gainReduction)
{
    inputMeter.setLevel(input);
    outputMeter.setLevel(output);
    gainReductionMeter.setLevel(gainReduction);
    latestGainReductionDb = gainReduction * 24.0f;
}

void F1Dashboard::setModuleStates(bool channelOn, bool compOn, bool airOn, bool delayOn, bool reverbOn)
{
    juce::ignoreUnused(reverbOn);
    moduleLeds[0].setOn(channelOn);
    moduleLeds[1].setOn(compOn);
    moduleLeds[2].setOn(airOn);
    moduleLeds[3].setOn(delayOn);
}

bool F1Dashboard::isCompressorPage() const noexcept
{
    return activePage == Page::comp;
}

juce::Rectangle<float> F1Dashboard::getWheelImageBounds() const
{
    auto area = getLocalBounds().toFloat().reduced(18.0f, 12.0f);

    if (! wheelImage.isValid())
        return area;

    const auto imageWidth = static_cast<float>(wheelImage.getWidth());
    const auto imageHeight = static_cast<float>(wheelImage.getHeight());
    const auto scale = juce::jmin(area.getWidth() / imageWidth, area.getHeight() / imageHeight);

    return juce::Rectangle<float>(imageWidth * scale, imageHeight * scale).withCentre(area.getCentre());
}

juce::Rectangle<float> F1Dashboard::getHotspotBounds(Hotspot hotspot) const
{
    const auto image = getWheelImageBounds();
    const auto x = image.getX();
    const auto y = image.getY();
    const auto w = image.getWidth();
    const auto h = image.getHeight();

    auto rel = [&] (float rx, float ry, float rw, float rh)
    {
        return juce::Rectangle<float>(x + w * rx, y + h * ry, w * rw, h * rh);
    };

    for (const auto& spec : hotspotMap)
        if (spec.hotspot == hotspot)
            return rel(spec.x, spec.y, spec.width, spec.height);

    return {};
}

juce::Rectangle<float> F1Dashboard::getWheelControlBounds(WheelControl control) const
{
    const auto image = getWheelImageBounds();
    const auto x = image.getX();
    const auto y = image.getY();
    const auto w = image.getWidth();
    const auto h = image.getHeight();

    for (const auto& spec : wheelControlMap)
    {
        if (spec.control == control)
            return { x + w * spec.x, y + h * spec.y, w * spec.width, h * spec.height };
    }

    return {};
}

bool F1Dashboard::isPointInsideWheelControl(WheelControl control, juce::Point<float> point) const
{
    const auto area = getWheelControlBounds(control);
    if (area.isEmpty())
        return false;

    for (const auto& spec : wheelControlMap)
    {
        if (spec.control != control)
            continue;

        if (! spec.roundHitZone)
            return area.contains(point);

        const auto centre = area.getCentre();
        const auto radiusX = area.getWidth() * 0.5f;
        const auto radiusY = area.getHeight() * 0.5f;

        if (radiusX <= 0.0f || radiusY <= 0.0f)
            return false;

        const auto normalizedX = (point.x - centre.x) / radiusX;
        const auto normalizedY = (point.y - centre.y) / radiusY;
        return normalizedX * normalizedX + normalizedY * normalizedY <= 1.0f;
    }

    return false;
}

juce::Rectangle<float> F1Dashboard::getControlPanelBounds() const
{
    auto image = getWheelImageBounds();
    auto panel = image.withTrimmedTop(image.getHeight() * 0.17f)
                      .withTrimmedBottom(image.getHeight() * 0.03f)
                      .reduced(image.getWidth() * 0.13f, 0.0f);

    if (activePage == Page::meters)
        panel = image.withTrimmedLeft(image.getWidth() * 0.62f)
                     .withTrimmedRight(image.getWidth() * 0.045f)
                     .withTrimmedTop(image.getHeight() * 0.20f)
                     .withTrimmedBottom(image.getHeight() * 0.08f);

    return panel;
}

F1Dashboard::Hotspot F1Dashboard::findHotspot(juce::Point<float> point) const
{
    for (const auto& spec : hotspotMap)
        if (getHotspotBounds(spec.hotspot).contains(point))
            return spec.hotspot;

    return Hotspot::none;
}

F1Dashboard::WheelControl F1Dashboard::findWheelControl(juce::Point<float> point) const
{
    for (const auto& spec : wheelControlMap)
        if (isPointInsideWheelControl(spec.control, point))
            return spec.control;

    return WheelControl::none;
}

F1Dashboard::Page F1Dashboard::pageForHotspot(Hotspot hotspot) const
{
    for (const auto& spec : hotspotMap)
        if (spec.hotspot == hotspot)
            return spec.page;

    return Page::global;
}

F1Dashboard::Hotspot F1Dashboard::hotspotForPage(Page page) const
{
    for (const auto& spec : hotspotMap)
        if (spec.page == page)
            return spec.hotspot;

    return Hotspot::global;
}

juce::String F1Dashboard::getHotspotLabel(Hotspot hotspot) const
{
    for (const auto& spec : hotspotMap)
        if (spec.hotspot == hotspot)
            return spec.label;

    return {};
}

juce::Colour F1Dashboard::getHotspotColour(Hotspot hotspot) const
{
    switch (hotspot)
    {
        case Hotspot::global:  return F1Theme::green();
        case Hotspot::channel: return F1Theme::green();
        case Hotspot::comp:    return F1Theme::red();
        case Hotspot::air:     return F1Theme::blue();
        case Hotspot::delay:   return F1Theme::amber();
        case Hotspot::routing: return F1Theme::cyan();
        case Hotspot::meters:  return F1Theme::violet();
        case Hotspot::none:    break;
    }

    return F1Theme::cyan();
}

juce::String F1Dashboard::getWheelControlLabel(WheelControl control) const
{
    if (const auto* zone = getWheelControlZone(control))
        return zone->label;

    return {};
}

const F1Dashboard::WheelControlZone* F1Dashboard::getWheelControlZone(WheelControl control) const
{
    for (const auto& spec : wheelControlMap)
        if (spec.control == control)
            return &spec;

    return nullptr;
}

juce::Colour F1Dashboard::getWheelControlColour(WheelControl control) const
{
    switch (control)
    {
        case WheelControl::input:
        case WheelControl::threshold:
        case WheelControl::midAir:
        case WheelControl::highAir:
        case WheelControl::reverbEnabled:
        case WheelControl::reverbMix:
            return F1Theme::green();

        case WheelControl::output:
        case WheelControl::ratio:
        case WheelControl::gain:
            return F1Theme::amber();

        case WheelControl::release:
        case WheelControl::attack:
            return F1Theme::blue();

        case WheelControl::feedback:
        case WheelControl::timing:
            return F1Theme::violet();

        case WheelControl::bypass:
            return F1Theme::red();

        case WheelControl::compEnabled:
            return F1Theme::red();

        case WheelControl::airEnabled:
            return F1Theme::green();

        case WheelControl::delayEnabled:
            return F1Theme::violet();

        case WheelControl::ab:
        case WheelControl::save:
            return F1Theme::cyan();

        case WheelControl::none:
            break;
    }

    return F1Theme::cyan();
}

F1Dashboard::WheelControlKind F1Dashboard::getWheelControlKind(WheelControl control) const
{
    if (const auto* zone = getWheelControlZone(control))
        return zone->kind;

    return WheelControlKind::placeholder;
}

GordoKnob* F1Dashboard::getKnobForWheelControl(WheelControl control) noexcept
{
    switch (control)
    {
        case WheelControl::input:     return &inputGain;
        case WheelControl::output:    return &outputGain;
        case WheelControl::threshold: return &compThreshold;
        case WheelControl::ratio:     return &compRatio;
        case WheelControl::release:   return &compRelease;
        case WheelControl::attack:    return &compAttack;
        case WheelControl::gain:      return &compOutput;
        case WheelControl::midAir:    return &airMidAir;
        case WheelControl::highAir:   return &airHighAir;
        case WheelControl::feedback:  return &delayPageSend;
        case WheelControl::timing:    return &delayDivision;
        case WheelControl::reverbMix: return &reverbPageMix;
        case WheelControl::none:
        case WheelControl::bypass:
        case WheelControl::compEnabled:
        case WheelControl::airEnabled:
        case WheelControl::delayEnabled:
        case WheelControl::reverbEnabled:
        case WheelControl::ab:
        case WheelControl::save:
            break;
    }

    return nullptr;
}

GordoButton* F1Dashboard::getButtonForWheelControl(WheelControl control) noexcept
{
    if (control == WheelControl::bypass)
        return &globalBypass;

    if (control == WheelControl::reverbEnabled)
        return &reverbEnabled;

    return nullptr;
}

juce::RangedAudioParameter* F1Dashboard::getParameterById(const juce::String& parameterId) const
{
    if (parameterState == nullptr)
    {
        DBG("Wheel UI mapping: APVTS pointer is null for " + parameterId);
        return nullptr;
    }

    if (parameterId.isEmpty())
        return nullptr;

    if (auto* parameter = parameterState->getParameter(parameterId))
        return parameter;

    DBG("Wheel UI mapping: parameter not found: " + parameterId);
    return nullptr;
}

float F1Dashboard::getParameterNormalizedById(const juce::String& parameterId) const
{
    if (auto* parameter = getParameterById(parameterId))
        return parameter->getValue();

    return 0.0f;
}

float F1Dashboard::getParameterRawValueById(const juce::String& parameterId) const
{
    if (parameterState == nullptr || parameterId.isEmpty())
        return 0.0f;

    if (auto* value = parameterState->getRawParameterValue(parameterId))
        return value->load();

    DBG("Wheel UI mapping: raw APVTS parameter not found: " + parameterId);
    return 0.0f;
}

juce::RangedAudioParameter* F1Dashboard::getWheelParameter(WheelControl control) const
{
    const auto parameterId = getWheelParameterId(control);
    auto* parameter = getParameterById(parameterId);

    if (parameter == nullptr && parameterId.isNotEmpty())
    {
        DBG("Wheel UI mapping: parameter lookup failed for " + getWheelControlLabel(control));
    }

    return parameter;
}

juce::String F1Dashboard::getWheelParameterId(WheelControl control) const
{
    if (const auto* zone = getWheelControlZone(control))
        return zone->parameterId;

    return {};
}

float F1Dashboard::getWheelParameterNormalized(WheelControl control) const
{
    if (auto* parameter = getWheelParameter(control))
        return parameter->getValue();

    return 0.0f;
}

int F1Dashboard::getWheelChoiceIndex(WheelControl control) const
{
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(getWheelParameter(control)))
        return choice->getIndex();

    if (auto* parameter = getWheelParameter(control))
        return juce::roundToInt(parameter->convertFrom0to1(parameter->getValue()));

    return 0;
}

int F1Dashboard::getWheelChoiceCount(WheelControl control) const
{
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(getWheelParameter(control)))
        return choice->choices.size();

    if (auto* parameter = getWheelParameter(control))
        return juce::roundToInt(parameter->convertFrom0to1(1.0f)) + 1;

    return 0;
}

void F1Dashboard::beginWheelParameterGesture(WheelControl control)
{
    activeWheelParameter = getWheelParameter(control);
    if (activeWheelParameter != nullptr)
        activeWheelParameter->beginChangeGesture();
}

void F1Dashboard::setWheelParameterNormalized(WheelControl control, float normalizedValue)
{
    auto* parameter = getWheelParameter(control);
    if (parameter == nullptr)
        return;

    const auto newValue = juce::jlimit(0.0f, 1.0f, normalizedValue);

    if (control == WheelControl::feedback)
    {
        setParameterNormalizedById("delayEnabled", 1.0f, "DELAY return ENABLE");
        primeDelayFeedbackForWheel();
    }
    else if (control == WheelControl::reverbMix && newValue > 0.0001f)
    {
        setParameterNormalizedById("reverbEnabled", 1.0f, "REVERB mix ENABLE");
        primeWheelReverbVoicingIfFactoryState();
    }

    const auto oldValue = parameter->getValue();
    parameter->setValueNotifyingHost(newValue);
    logWheelFunctionalMapping(control, oldValue, newValue);

    if (control == WheelControl::midAir || control == WheelControl::highAir)
        showAudibleStatusTooltip(control, getAirWheelDiagnosticText(control));
}

void F1Dashboard::setWheelChoiceIndex(WheelControl control, int index)
{
    auto* parameter = getWheelParameter(control);
    if (parameter == nullptr)
        return;

    const auto count = getWheelChoiceCount(control);
    if (count <= 0)
        return;

    const auto oldValue = parameter->getValue();
    const auto safeIndex = juce::jlimit(0, count - 1, index);
    const auto newValue = [parameter, safeIndex]
    {
        if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(parameter))
            return choice->convertTo0to1(static_cast<float>(safeIndex));

        return parameter->convertTo0to1(static_cast<float>(safeIndex));
    }();

    parameter->setValueNotifyingHost(newValue);
    logWheelFunctionalMapping(control, oldValue, newValue);
}

void F1Dashboard::endWheelParameterGesture()
{
    if (activeWheelParameter != nullptr)
    {
        activeWheelParameter->endChangeGesture();
        activeWheelParameter = nullptr;
    }
}

void F1Dashboard::logWheelFunctionalMapping(WheelControl control, float oldNormalized, float newNormalized)
{
    if constexpr (! debugWheelFunctionalMapping)
    {
        juce::ignoreUnused(control, oldNormalized, newNormalized);
    }
    else
    {
        const auto label = getWheelControlLabel(control);
        const auto parameterId = getWheelParameterId(control);
        auto* parameter = getWheelParameter(control);
        const auto oldActual = parameter != nullptr ? parameter->convertFrom0to1(oldNormalized) : 0.0f;
        const auto newActual = parameter != nullptr ? parameter->convertFrom0to1(newNormalized) : 0.0f;
        juce::ignoreUnused(oldActual, newActual);
        auto getModuleEnabledId = [] (WheelControl wheelControl) -> juce::String
        {
            switch (wheelControl)
            {
                case WheelControl::threshold:
                case WheelControl::ratio:
                case WheelControl::release:
                case WheelControl::attack:
                case WheelControl::gain:
                case WheelControl::compEnabled:
                    return "compEnabled";

                case WheelControl::midAir:
                case WheelControl::highAir:
                case WheelControl::airEnabled:
                    return "airEnabled";

                case WheelControl::feedback:
                case WheelControl::timing:
                case WheelControl::delayEnabled:
                    return "delayEnabled";

                case WheelControl::reverbEnabled:
                case WheelControl::reverbMix:
                    return "reverbEnabled";

                default:
                    return {};
            }
        };

        auto getModuleMixSendId = [] (WheelControl wheelControl) -> juce::String
        {
            switch (wheelControl)
            {
                case WheelControl::threshold:
                case WheelControl::ratio:
                case WheelControl::release:
                case WheelControl::attack:
                case WheelControl::gain:
                case WheelControl::compEnabled:
                    return "compMix";

                case WheelControl::midAir:
                case WheelControl::highAir:
                case WheelControl::airEnabled:
                    return "airMix";

                case WheelControl::feedback:
                case WheelControl::timing:
                case WheelControl::delayEnabled:
                    return "delaySend";

                case WheelControl::reverbEnabled:
                case WheelControl::reverbMix:
                    return "reverbMix";

                default:
                    return {};
            }
        };

        const auto moduleEnabledId = getModuleEnabledId(control);
        const auto mixSendId = getModuleMixSendId(control);
        const auto moduleEnabledText = moduleEnabledId.isNotEmpty()
                                     ? moduleEnabledId + "=" + (getParameterNormalizedById(moduleEnabledId) >= 0.5f ? "ON" : "OFF")
                                     : juce::String("module=n/a");
        const auto mixSendText = mixSendId.isNotEmpty()
                               ? mixSendId + "=" + juce::String(getParameterNormalizedById(mixSendId) * 100.0f, 1) + "%"
                               : juce::String("mixSend=n/a");
        const auto rawText = "rawAfter=" + juce::String(getParameterRawValueById(parameterId), 4);
        const auto valueText = [parameter] (float normalized)
        {
            if (parameter == nullptr)
                return juce::String("n/a");

            const auto text = parameter->getText(normalized, 48);
            if (text.isNotEmpty())
                return text;

            return juce::String(parameter->convertFrom0to1(normalized), 4);
        };
        const auto moduleStateValue = moduleEnabledId.isNotEmpty()
                                    ? (getParameterNormalizedById(moduleEnabledId) >= 0.5f ? "ON" : "OFF")
                                    : juce::String("n/a");
        const auto mixSendValue = mixSendId.isNotEmpty()
                                ? juce::String(juce::roundToInt(getParameterNormalizedById(mixSendId) * 100.0f)) + "%"
                                : juce::String("n/a");
        const auto foundText = parameter != nullptr ? "PARAMETER FOUND" : "PARAMETER MISSING";
        auto mapCompressorAttackMs = [] (float attackMs)
        {
            const auto normalised = juce::jlimit(0.0f, 1.0f, (attackMs - 0.02f) / (2.0f - 0.02f));
            return 0.05f + std::pow(normalised, 1.35f) * 34.95f;
        };
        auto mapCompressorReleaseMs = [] (float releaseMs)
        {
            const auto normalised = juce::jlimit(0.0f, 1.0f, (releaseMs - 50.0f) / (1200.0f - 50.0f));
            return 35.0f + std::pow(normalised, 1.12f) * 1565.0f;
        };
        const auto airMidValue = juce::jlimit(0.0f, 1.0f, getParameterRawValueById("airMidAir"));
        const auto airHighValue = juce::jlimit(0.0f, 1.0f, getParameterRawValueById("airHighAir"));
        const auto airMidIntensity = std::pow(airMidValue, 0.72f);
        const auto airHighIntensity = std::pow(airHighValue, 0.72f);
        const auto airDemand = juce::jlimit(0.0f, 1.0f, juce::jmax(airMidIntensity, airHighIntensity));
        const auto airAmount = juce::jlimit(0.0f, 1.0f, getParameterRawValueById("airAmount"));
        const auto airDensityValue = juce::jlimit(0.0f, 1.0f, getParameterRawValueById("airDensity"));
        const auto airColourAmount = 0.35f + airAmount * 0.65f;
        const auto airEffectiveDensity = juce::jlimit(0.0f, 1.0f, juce::jmax(airDensityValue, airDemand * 0.35f));
        const auto effectiveMidAir = airMidIntensity * (0.70f + airColourAmount * 0.72f + airEffectiveDensity * 0.42f);
        const auto effectiveHighAir = airHighIntensity * (0.90f + airColourAmount * 0.92f + airEffectiveDensity * 0.48f);
        const auto compressorBridgeText = " compEnabled=" + juce::String(getParameterRawValueById("compEnabled"), 1)
                                        + " compMix=" + juce::String(getParameterRawValueById("compMix") * 100.0f, 1) + "%"
                                        + " compThresholdDb=" + juce::String(getParameterRawValueById("compThresholdDb"), 2)
                                        + " compGainDb=" + juce::String(getParameterRawValueById("compOutputDb"), 2)
                                        + " compRatioIndex=" + juce::String(getParameterRawValueById("compRatio"), 1)
                                        + " compRatioReal=" + juce::String([this]
                                          {
                                              switch (juce::jlimit(0, 4, juce::roundToInt(getParameterRawValueById("compRatio"))))
                                              {
                                                  case 1: return 8.0f;
                                                  case 2: return 12.0f;
                                                  case 3: return 20.0f;
                                                  case 4: return 30.0f;
                                                  default: return 4.0f;
                                              }
                                          }(), 1)
                                        + " compAttackMs=" + juce::String(getParameterRawValueById("compAttack"), 4)
                                        + " compAttackEffectiveMs=" + juce::String(mapCompressorAttackMs(getParameterRawValueById("compAttack")), 2)
                                        + " compReleaseMs=" + juce::String(getParameterRawValueById("compRelease"), 2)
                                        + " compReleaseEffectiveMs=" + juce::String(mapCompressorReleaseMs(getParameterRawValueById("compRelease")), 1)
                                        + " compGRApproxDb=" + juce::String(latestGainReductionDb, 2);
        const auto airBridgeText = " airEnabled=" + juce::String(getParameterRawValueById("airEnabled"), 1)
                                 + " airMix=" + juce::String(getParameterRawValueById("airMix") * 100.0f, 1) + "%"
                                 + " airAmount=" + juce::String(getParameterRawValueById("airAmount") * 100.0f, 1) + "%"
                                 + " airDrive=" + juce::String(getParameterRawValueById("airDrive") * 100.0f, 1) + "%"
                                 + " airMidAir=" + juce::String(getParameterRawValueById("airMidAir") * 100.0f, 1) + "%"
                                 + " airHighAir=" + juce::String(getParameterRawValueById("airHighAir") * 100.0f, 1) + "%"
                                 + " effectiveMidAir=" + juce::String(effectiveMidAir * 100.0f, 1) + "%"
                                 + " effectiveHighAir=" + juce::String(effectiveHighAir * 100.0f, 1) + "%";
        auto getDelayDivisionText = [this]
        {
            switch (juce::jlimit(0, 7, juce::roundToInt(getParameterRawValueById("delayNoteDivision"))))
            {
                case 1: return juce::String("1/8");
                case 2: return juce::String("1/8D");
                case 3: return juce::String("1/8T");
                case 4: return juce::String("1/16");
                case 5: return juce::String("1/16D");
                case 6: return juce::String("1/16T");
                case 7: return juce::String("1/2");
                default: return juce::String("1/4");
            }
        };
        const auto delayBridgeText = " delayEnabled=" + juce::String(getParameterRawValueById("delayEnabled"), 1)
                                   + " delaySend=" + juce::String(getParameterRawValueById("delaySend") * 100.0f, 1) + "%"
                                   + " delayFeedbackInternal=" + juce::String(getParameterRawValueById("delayFeedback") * 100.0f, 1) + "%"
                                   + " delayNoteDivision=" + getDelayDivisionText()
                                   + " delaySync=" + juce::String(getParameterRawValueById("delaySyncEnabled"), 1)
                                   + " delayTimeMsParam=" + juce::String(getParameterRawValueById("delayTimeMs"), 1);
        const auto reverbBridgeText = " reverbEnabled=" + juce::String(getParameterRawValueById("reverbEnabled"), 1)
                                    + " reverbMix=" + juce::String(getParameterRawValueById("reverbMix") * 100.0f, 1) + "%"
                                    + " reverbDecaySec=" + juce::String(getParameterRawValueById("reverbDecaySec"), 2)
                                    + " reverbSize=" + juce::String(getParameterRawValueById("reverbSize") * 100.0f, 1) + "%"
                                    + " reverbPredelayMs=" + juce::String(getParameterRawValueById("reverbPredelayMs"), 1)
                                    + " reverbLowCutHz=" + juce::String(getParameterRawValueById("reverbLowCutHz"), 0)
                                    + " reverbHighCutHz=" + juce::String(getParameterRawValueById("reverbHighCutHz"), 0)
                                    + " reverbHighDampDb=" + juce::String(getParameterRawValueById("reverbHighDampDb"), 1)
                                    + " reverbBypass=" + juce::String(getParameterRawValueById("globalBypass"), 1);
        const auto moduleDetailText = [control, &delayBridgeText, &reverbBridgeText]
        {
            switch (control)
            {
                case WheelControl::feedback:
                case WheelControl::timing:
                case WheelControl::delayEnabled:
                    return delayBridgeText;

                case WheelControl::reverbEnabled:
                case WheelControl::reverbMix:
                    return reverbBridgeText;

                default:
                    return juce::String();
            }
        }();

        debugOverlayText = label + " -> " + (parameterId.isNotEmpty() ? parameterId : juce::String("(none)")) + "\n"
                         + "old: " + valueText(oldNormalized) + "  new: " + valueText(newNormalized) + "\n"
                         + "module: " + moduleStateValue + "  mix/send: " + mixSendValue + "\n"
                         + foundText
                         + (moduleDetailText.isNotEmpty() ? "\n" + moduleDetailText : juce::String());
        debugOverlayUntilMs = juce::Time::getMillisecondCounter() + 3600u;

        DBG("Wheel UI mapping: " + label
            + " -> " + parameterId
            + " oldNorm=" + juce::String(oldNormalized, 4)
            + " newNorm=" + juce::String(newNormalized, 4)
            + " oldValue=" + juce::String(oldActual, 4)
            + " newValue=" + juce::String(newActual, 4)
            + " text=" + getWheelControlValueText(control)
            + " found=" + (parameter != nullptr ? "yes" : "no")
            + " " + rawText
            + " " + moduleEnabledText
            + " " + mixSendText
            + compressorBridgeText
            + airBridgeText
            + moduleDetailText);
    }
}

void F1Dashboard::beginWheelControlDrag(WheelControl control, juce::Point<float> position)
{
    activeWheelControl = control;
    dragStartPosition = position;
    makeOwningModuleAudibleForControl(control);
    ensureAirAudibleForWheelTest(control, true);
    dragStartNormalized = getWheelParameterNormalized(control);
    dragStartChoiceIndex = getWheelChoiceIndex(control);
    beginWheelParameterGesture(control);

    showWheelControlTooltip(control);
    repaint();
}

void F1Dashboard::dragWheelControl(juce::Point<float> position)
{
    if (getWheelParameter(activeWheelControl) == nullptr)
        return;

    makeOwningModuleAudibleForControl(activeWheelControl);

    const auto movement = static_cast<double>((dragStartPosition.y - position.y) + (position.x - dragStartPosition.x) * 0.35f);
    const auto kind = getWheelControlKind(activeWheelControl);

    if (kind == WheelControlKind::timing)
    {
        const auto steps = static_cast<int>(std::round(movement / 42.0));
        setTimingStepIndex(getTimingStepIndex(dragStartNormalized) + steps);
        showWheelControlTooltip(activeWheelControl);
        repaint();
        return;
    }

    if (kind == WheelControlKind::stepped)
    {
        const auto count = getWheelChoiceCount(activeWheelControl);
        const auto steps = static_cast<int>(std::round(movement / 42.0));
        if (count > 0)
            setWheelChoiceIndex(activeWheelControl, dragStartChoiceIndex + steps);
    }
    else
    {
        const auto value = dragStartNormalized + static_cast<float>(movement / 260.0);
        setWheelParameterNormalized(activeWheelControl, value);
    }

    showWheelControlTooltip(activeWheelControl);
    repaint();
}

void F1Dashboard::endWheelControlDrag()
{
    endWheelParameterGesture();
    activeWheelControl = WheelControl::none;
    setMouseCursor(hoveredWheelControl == WheelControl::none ? juce::MouseCursor::NormalCursor
                                                             : juce::MouseCursor::PointingHandCursor);
    repaint();
}

void F1Dashboard::incrementWheelControl(WheelControl control, int direction)
{
    if (control == WheelControl::none)
        return;

    if (getWheelControlKind(control) == WheelControlKind::timing)
    {
        cycleTiming(direction);
        return;
    }

    if (getWheelControlKind(control) == WheelControlKind::toggle)
    {
        toggleWheelControl(control);
        return;
    }

    if (getWheelControlKind(control) == WheelControlKind::placeholder)
    {
        showWheelControlTooltip(control);
        repaint();
        return;
    }

    auto* parameter = getWheelParameter(control);
    if (parameter == nullptr)
        return;

    makeOwningModuleAudibleForControl(control);
    ensureAirAudibleForWheelTest(control, true);

    parameter->beginChangeGesture();
    if (getWheelControlKind(control) == WheelControlKind::stepped)
        setWheelChoiceIndex(control, getWheelChoiceIndex(control) + direction);
    else
        setWheelParameterNormalized(control, parameter->getValue() + static_cast<float>(direction) * 0.025f);
    parameter->endChangeGesture();

    showWheelControlTooltip(control);
    repaint();
}

int F1Dashboard::getTimingStepIndex(double normalizedValue) const
{
    auto* parameter = getWheelParameter(WheelControl::timing);
    if (parameter == nullptr)
        return 0;

    const auto actualValue = parameter->convertFrom0to1(static_cast<float>(normalizedValue));
    constexpr std::array<double, 4> timingValues { 0.0, 1.0, 3.0, 4.0 };
    auto currentIndex = 0;
    auto bestDistance = std::numeric_limits<double>::max();

    for (auto i = 0; i < static_cast<int>(timingValues.size()); ++i)
    {
        const auto distance = std::abs(actualValue - timingValues[static_cast<size_t>(i)]);
        if (distance < bestDistance)
        {
            bestDistance = distance;
            currentIndex = i;
        }
    }

    return currentIndex;
}

void F1Dashboard::setTimingStepIndex(int index)
{
    if (getWheelParameter(WheelControl::timing) == nullptr)
        return;

    constexpr std::array<double, 4> timingValues { 0.0, 1.0, 3.0, 4.0 };
    const auto safeIndex = juce::jlimit(0, static_cast<int>(timingValues.size()) - 1, index);
    const auto currentIndex = getTimingStepIndex(getWheelParameterNormalized(WheelControl::timing));

    if (safeIndex == currentIndex)
        return;

    primeDelayTimingForWheel();
    setWheelChoiceIndex(WheelControl::timing, juce::roundToInt(timingValues[static_cast<size_t>(safeIndex)]));
}

void F1Dashboard::cycleTiming(int direction)
{
    auto* parameter = getWheelParameter(WheelControl::timing);
    if (parameter == nullptr)
        return;

    const auto stepCount = 4;
    const auto nextIndex = (getTimingStepIndex(parameter->getValue()) + (direction >= 0 ? 1 : -1) + stepCount) % stepCount;
    parameter->beginChangeGesture();
    setTimingStepIndex(nextIndex);
    parameter->endChangeGesture();
    showWheelControlTooltip(WheelControl::timing);
    repaint();
}

void F1Dashboard::setParameterNormalizedById(const juce::String& parameterId,
                                             float normalizedValue,
                                             const juce::String& labelForDebug)
{
    auto* parameter = getParameterById(parameterId);
    if (parameter == nullptr)
        return;

    const auto oldValue = parameter->getValue();
    const auto newValue = juce::jlimit(0.0f, 1.0f, normalizedValue);

    if (std::abs(oldValue - newValue) <= 0.0001f)
    {
        if constexpr (debugWheelFunctionalMapping)
        {
            DBG("Wheel UI mapping: " + labelForDebug
                + " -> " + parameterId
                + " unchangedNorm=" + juce::String(newValue, 4)
                + " rawAfter=" + juce::String(getParameterRawValueById(parameterId), 4)
                + " found=yes");
        }

        return;
    }

    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost(newValue);
    parameter->endChangeGesture();

    if constexpr (debugWheelFunctionalMapping)
    {
        DBG("Wheel UI mapping: " + labelForDebug
            + " -> " + parameterId
            + " oldNorm=" + juce::String(oldValue, 4)
            + " newNorm=" + juce::String(newValue, 4)
            + " rawAfter=" + juce::String(getParameterRawValueById(parameterId), 4)
            + " found=yes");
    }
    else
    {
        juce::ignoreUnused(labelForDebug, oldValue);
    }
}

void F1Dashboard::setParameterActualValueById(const juce::String& parameterId,
                                              float actualValue,
                                              const juce::String& labelForDebug)
{
    if (auto* parameter = getParameterById(parameterId))
        setParameterNormalizedById(parameterId, parameter->convertTo0to1(actualValue), labelForDebug);
}

void F1Dashboard::primeDelayFeedbackForWheel()
{
    if (getParameterRawValueById("delayFeedback") < 0.12f)
        setParameterNormalizedById("delayFeedback", 0.38f, "DELAY auto INTERNAL FEEDBACK");
}

void F1Dashboard::primeDelayTimingForWheel()
{
    setParameterNormalizedById("delayEnabled", 1.0f, "DELAY timing ENABLE");
    setParameterNormalizedById("delaySyncEnabled", 1.0f, "DELAY timing SYNC");

    if (getParameterRawValueById("delaySend") <= 0.0001f)
        setParameterNormalizedById("delaySend", 0.35f, "DELAY timing RETURN");

    primeDelayFeedbackForWheel();
}

void F1Dashboard::primeWheelReverbVoicingIfFactoryState()
{
    auto nearRawValue = [this] (const juce::String& parameterId, float targetValue, float tolerance)
    {
        return std::abs(getParameterRawValueById(parameterId) - targetValue) <= tolerance;
    };

    const auto looksLikeFactoryPreset = juce::roundToInt(getParameterRawValueById("reverbMode")) == 1
                                     && juce::roundToInt(getParameterRawValueById("reverbColor")) == 0
                                     && nearRawValue("reverbPredelayMs", 14.0f, 0.5f)
                                     && nearRawValue("reverbDecaySec", 1.85f, 0.05f)
                                     && nearRawValue("reverbSize", 0.50f, 0.02f);

    const auto looksLikePreviousWheelPreset = juce::roundToInt(getParameterRawValueById("reverbMode")) == 4
                                           && juce::roundToInt(getParameterRawValueById("reverbColor")) == 0
                                           && nearRawValue("reverbPredelayMs", 26.0f, 1.0f)
                                           && nearRawValue("reverbDecaySec", 2.55f, 0.08f)
                                           && nearRawValue("reverbSize", 0.72f, 0.03f)
                                           && nearRawValue("reverbLowCutHz", 190.0f, 8.0f);

    if (! looksLikeFactoryPreset && ! looksLikePreviousWheelPreset)
        return;

    setParameterActualValueById("reverbPredelayMs", 46.0f, "REVERB wheel PREDELAY");
    setParameterActualValueById("reverbDecaySec", 3.65f, "REVERB wheel DECAY");
    setParameterNormalizedById("reverbSize", 0.82f, "REVERB wheel SIZE");
    setParameterNormalizedById("reverbAttack", 0.02f, "REVERB wheel ATTACK");
    setParameterNormalizedById("reverbEarly", 0.16f, "REVERB wheel EARLY");
    setParameterNormalizedById("reverbLate", 0.94f, "REVERB wheel LATE");
    setParameterActualValueById("reverbLowCutHz", 340.0f, "REVERB wheel LOW CUT");
    setParameterActualValueById("reverbHighCutHz", 9200.0f, "REVERB wheel HIGH CUT");
    setParameterActualValueById("reverbLowDampHz", 430.0f, "REVERB wheel LOW DAMP");
    setParameterActualValueById("reverbHighDampDb", -10.5f, "REVERB wheel HIGH DAMP");
    setParameterNormalizedById("reverbDiffusionEarly", 0.78f, "REVERB wheel EARLY DIFF");
    setParameterNormalizedById("reverbDiffusionLate", 0.97f, "REVERB wheel LATE DIFF");
    setParameterActualValueById("reverbModRate", 0.16f, "REVERB wheel MOD RATE");
    setParameterNormalizedById("reverbModDepth", 0.20f, "REVERB wheel MOD DEPTH");
    setParameterNormalizedById("reverbWidth", 0.98f, "REVERB wheel WIDTH");
    setParameterActualValueById("reverbMode", 4.0f, "REVERB wheel MODE");
    setParameterActualValueById("reverbColor", 0.0f, "REVERB wheel COLOR");
    setParameterNormalizedById("reverbDucking", 0.06f, "REVERB wheel DUCKING");
    setParameterNormalizedById("reverbMonoBass", 1.0f, "REVERB wheel MONO BASS");
    setParameterNormalizedById("reverbTempoSyncPredelay", 0.0f, "REVERB wheel SYNC PRE");
}

void F1Dashboard::makeModuleAudibleIfNeeded(WheelControl control)
{
    struct AudibleAmount
    {
        WheelControl control;
        const char* parameterId;
        float targetNormalized;
        const char* label;
    };

    static constexpr std::array<AudibleAmount, 4> audibleAmounts {{
        { WheelControl::compEnabled,  "compMix",   1.00f, "COMP auto MIX"  },
        { WheelControl::airEnabled,   "airMix",    1.00f, "AIR auto MIX"   },
        { WheelControl::delayEnabled, "delaySend", 0.35f, "DELAY auto SEND" },
        { WheelControl::reverbEnabled, "reverbMix", 0.30f, "REVERB auto MIX" }
    }};

    for (const auto& amount : audibleAmounts)
    {
        if (amount.control != control)
            continue;

        if (auto* parameter = getParameterById(amount.parameterId))
        {
            if (parameter->getValue() <= 0.0001f)
                setParameterNormalizedById(amount.parameterId, amount.targetNormalized, amount.label);
        }

        if (control == WheelControl::airEnabled
            && getParameterRawValueById("airAmount") <= 0.0001f)
        {
            setParameterNormalizedById("airAmount", 1.0f, "AIR auto AMOUNT");
        }

        if (control == WheelControl::airEnabled
            && getParameterRawValueById("airDrive") <= 0.0001f)
        {
            setParameterNormalizedById("airDrive", 0.5f, "AIR auto INTENSITY");
        }

        if (control == WheelControl::delayEnabled)
            primeDelayFeedbackForWheel();

        if (control == WheelControl::reverbEnabled)
            primeWheelReverbVoicingIfFactoryState();

        return;
    }
}

void F1Dashboard::makeOwningModuleAudibleForControl(WheelControl control)
{
    switch (control)
    {
        case WheelControl::ratio:
        case WheelControl::release:
        case WheelControl::attack:
        {
            const auto needsEnable = getParameterNormalizedById("compEnabled") < 0.5f;
            const auto needsMix = getParameterNormalizedById("compMix") <= 0.0001f;
            const auto needsThreshold = getParameterRawValueById("compThresholdDb") > -18.0f;

            setParameterNormalizedById("compEnabled", 1.0f, "COMP auto ENABLE");
            makeModuleAudibleIfNeeded(WheelControl::compEnabled);
            if (needsThreshold)
            {
                if (auto* threshold = getParameterById("compThresholdDb"))
                    setParameterNormalizedById("compThresholdDb",
                                               threshold->convertTo0to1(-24.0f),
                                               "COMP auto THRESHOLD");
            }

            if (needsEnable || needsMix || needsThreshold)
            {
                showAudibleStatusTooltip(control,
                                         "COMP ACTIVE / THRESHOLD "
                                             + juce::String(getParameterRawValueById("compThresholdDb"), 0)
                                             + " dB");
            }
            return;
        }

        case WheelControl::midAir:
        case WheelControl::highAir:
            ensureAirAudibleForWheelTest(control, false);
            return;

        default:
            break;
    }
}

void F1Dashboard::ensureAirAudibleForWheelTest(WheelControl control, bool primeControlledValue)
{
    if (control != WheelControl::midAir && control != WheelControl::highAir)
        return;

    setParameterNormalizedById("airEnabled", 1.0f, "AIR test ENABLE");
    setParameterNormalizedById("airMix", 1.0f, "AIR test MIX");
    setParameterNormalizedById("airAmount", 1.0f, "AIR test AMOUNT");

    if (getParameterRawValueById("airDrive") <= 0.0001f)
        setParameterNormalizedById("airDrive", 0.5f, "AIR test INTENSITY");

    if (primeControlledValue && getWheelParameterNormalized(control) <= 0.0001f)
    {
        if (auto* parameter = getWheelParameter(control))
        {
            parameter->beginChangeGesture();
            setWheelParameterNormalized(control, 0.75f);
            parameter->endChangeGesture();
        }
    }

    showAudibleStatusTooltip(control, getAirWheelDiagnosticText(control));
}

void F1Dashboard::toggleWheelControl(WheelControl control)
{
    auto* parameter = getWheelParameter(control);
    if (parameter == nullptr)
        return;

    const auto turningOn = parameter->getValue() < 0.5f;

    parameter->beginChangeGesture();
    setWheelParameterNormalized(control, turningOn ? 1.0f : 0.0f);
    parameter->endChangeGesture();

    if (turningOn)
        makeModuleAudibleIfNeeded(control);

    showWheelControlTooltip(control);
    repaint();
}

void F1Dashboard::showWheelControlTooltip(WheelControl control)
{
    if (control == WheelControl::none)
        return;

    tooltipUntilMs = juce::Time::getMillisecondCounter() + 1600u;
}

void F1Dashboard::showAudibleStatusTooltip(WheelControl control, const juce::String& text)
{
    if (control == WheelControl::none)
        return;

    audibleStatusControl = control;
    audibleStatusTooltip = text;
    audibleStatusTooltipUntilMs = juce::Time::getMillisecondCounter() + 1600u;
    tooltipUntilMs = audibleStatusTooltipUntilMs;
}

juce::String F1Dashboard::getWheelControlValueText(WheelControl control) const
{
    if (control == WheelControl::ab || control == WheelControl::save)
        return "PLACEHOLDER";

    if (control == WheelControl::bypass)
        return getWheelParameterNormalized(control) >= 0.5f ? "ON" : "OFF";

    if (control == WheelControl::reverbEnabled)
        return getWheelParameterNormalized(control) >= 0.5f ? "ON" : "OFF";

    if (control == WheelControl::feedback)
        return juce::String(juce::roundToInt(getWheelParameterNormalized(control) * 100.0f)) + "%";

    if (control == WheelControl::reverbMix)
        return juce::String(juce::roundToInt(getWheelParameterNormalized(control) * 100.0f)) + "%";

    if (control == WheelControl::compEnabled || control == WheelControl::airEnabled || control == WheelControl::delayEnabled)
        return getModuleAudibleStateText(control);

    if (control == WheelControl::midAir || control == WheelControl::highAir)
        return getAirWheelDiagnosticText(control);

    if (control == WheelControl::ratio)
    {
        if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(getWheelParameter(control)))
        {
            const auto index = juce::jlimit(0, choice->choices.size() - 1, choice->getIndex());
            return choice->choices[index];
        }
    }

    if (control == WheelControl::timing)
    {
        if (auto* parameter = getWheelParameter(control))
        {
            const auto value = juce::roundToInt(parameter->convertFrom0to1(parameter->getValue()));
            if (value == 0) return "1/4";
            if (value == 1) return "1/8";
            if (value == 3) return "1/12";
            if (value == 4) return "1/16";
            return parameter->getCurrentValueAsText();
        }
    }

    if (auto* parameter = getWheelParameter(control))
        return parameter->getCurrentValueAsText();

    return {};
}

juce::String F1Dashboard::getAirWheelDiagnosticText(WheelControl control) const
{
    if (control != WheelControl::midAir && control != WheelControl::highAir)
        return {};

    const auto controlPercent = juce::roundToInt(getWheelParameterNormalized(control) * 100.0f);
    const auto airOn = getParameterNormalizedById("airEnabled") >= 0.5f ? "AIR ON" : "AIR OFF";
    const auto airMixPercent = juce::roundToInt(getParameterNormalizedById("airMix") * 100.0f);
    const auto airAmountPercent = juce::roundToInt(getParameterRawValueById("airAmount") * 100.0f);
    const auto airDrivePercent = juce::roundToInt(getParameterRawValueById("airDrive") * 100.0f);

    return getWheelControlLabel(control) + " " + juce::String(controlPercent) + "%"
        + " / " + airOn
        + " / MIX " + juce::String(airMixPercent) + "%"
        + " / AMOUNT " + juce::String(airAmountPercent) + "%"
        + " / DRIVE " + juce::String(airDrivePercent) + "%";
}

juce::String F1Dashboard::getModuleAudibleStateText(WheelControl control) const
{
    if (parameterState == nullptr || getWheelParameterNormalized(control) < 0.5f)
        return "OFF";

    auto getPercentValue = [this] (const juce::String& parameterId)
    {
        if (auto* parameter = getParameterById(parameterId))
            return juce::roundToInt(parameter->convertFrom0to1(parameter->getValue()) * 100.0f);

        DBG("Wheel UI mapping: module mix/send parameter not found: " + parameterId);
        return 0;
    };

    if (control == WheelControl::compEnabled)
        return "ON / MIX " + juce::String(getPercentValue("compMix")) + "%";

    if (control == WheelControl::airEnabled)
        return "ON / MIX " + juce::String(getPercentValue("airMix")) + "%";

    if (control == WheelControl::delayEnabled)
        return "ON / SEND " + juce::String(getPercentValue("delaySend")) + "%";

    if (control == WheelControl::reverbEnabled)
        return "ON / MIX " + juce::String(getPercentValue("reverbMix")) + "%";

    return "ON";
}

void F1Dashboard::loadWheelPointerImages()
{
    auto imageFromMemory = [] (const void* data, int size)
    {
        return juce::ImageCache::getFromMemory(data, size);
    };

    wheelPointers = {{
        { WheelControl::input,     "input",     "inputGainDb",       imageFromMemory(BinaryData::input_pointer_png,     BinaryData::input_pointer_pngSize),     { 0.234f, 0.090f }, { 0.207f, 0.050f, 0.054f, 0.080f }, -135.0f,  135.0f, false },
        { WheelControl::output,    "output",    "outputGainDb",      imageFromMemory(BinaryData::output_pointer_png,    BinaryData::output_pointer_pngSize),    { 0.763f, 0.090f }, { 0.736f, 0.050f, 0.054f, 0.080f }, -135.0f,  135.0f, false },
        { WheelControl::threshold, "threshold", "compThresholdDb",   imageFromMemory(BinaryData::threshold_pointer_png, BinaryData::threshold_pointer_pngSize), { 0.382f, 0.468f }, { 0.348f, 0.365f, 0.068f, 0.160f }, -135.0f,  135.0f, false },
        { WheelControl::ratio,     "ratio",     "compRatio",         imageFromMemory(BinaryData::ratio_pointer_png,     BinaryData::ratio_pointer_pngSize),     { 0.442f, 0.266f }, { 0.408f, 0.190f, 0.068f, 0.150f }, -135.0f,  135.0f, true  },
        { WheelControl::release,   "release",   "compRelease",       imageFromMemory(BinaryData::release_pointer_png,   BinaryData::release_pointer_pngSize),   { 0.562f, 0.270f }, { 0.530f, 0.193f, 0.064f, 0.150f }, -120.0f,  150.0f, false },
        { WheelControl::attack,    "attack",    "compAttack",        imageFromMemory(BinaryData::attack_pointer_png,    BinaryData::attack_pointer_pngSize),    { 0.616f, 0.470f }, { 0.586f, 0.390f, 0.060f, 0.150f }, -125.0f,  145.0f, false },
        { WheelControl::gain,      "gain",      "compOutputDb",      imageFromMemory(BinaryData::gain_pointer_png,      BinaryData::gain_pointer_pngSize),      { 0.500f, 0.681f }, { 0.466f, 0.596f, 0.068f, 0.170f }, -135.0f,  135.0f, false },
        { WheelControl::midAir,    "midair",    "airMidAir",         imageFromMemory(BinaryData::midair_pointer_png,    BinaryData::midair_pointer_pngSize),    { 0.293f, 0.586f }, { 0.276f, 0.535f, 0.034f, 0.102f }, -135.0f,  135.0f, false },
        { WheelControl::highAir,   "highair",   "airHighAir",        imageFromMemory(BinaryData::highair_pointer_png,   BinaryData::highair_pointer_pngSize),   { 0.293f, 0.686f }, { 0.276f, 0.635f, 0.034f, 0.102f }, -135.0f,  135.0f, false },
        { WheelControl::feedback,  "feedback",  "delaySend",         imageFromMemory(BinaryData::feedback_pointer_png,  BinaryData::feedback_pointer_pngSize),  { 0.710f, 0.551f }, { 0.692f, 0.500f, 0.036f, 0.102f }, -135.0f,  135.0f, false },
        { WheelControl::timing,    "timing",    "delayNoteDivision", imageFromMemory(BinaryData::timing_pointer_png,    BinaryData::timing_pointer_pngSize),    { 0.710f, 0.687f }, { 0.692f, 0.636f, 0.036f, 0.102f },  -90.0f,  180.0f, true  }
    }};
}

float F1Dashboard::getWheelPointerAngleDegrees(const WheelPointer& pointer)
{
    if (pointer.control == WheelControl::timing)
    {
        if (auto* parameter = getWheelParameter(pointer.control))
        {
            const auto value = juce::roundToInt(parameter->convertFrom0to1(parameter->getValue()));
            if (value == 0) return -90.0f;  // 1/4
            if (value == 1) return   0.0f;  // 1/8
            if (value == 3) return  90.0f;  // 1/12 via 1/8T
            if (value == 4) return 180.0f;  // 1/16
        }

        return 0.0f;
    }

    auto* parameter = getWheelParameter(pointer.control);
    if (parameter == nullptr)
        return 0.0f;

    auto proportion = parameter->getValue();

    return juce::jmap(proportion, pointer.minAngleDegrees, pointer.maxAngleDegrees);
}

void F1Dashboard::paintWheelPointers(juce::Graphics& g, juce::Rectangle<float> imageBounds)
{
    for (auto& pointer : wheelPointers)
    {
        if (! pointer.image.isValid())
            continue;

        const auto bounds = juce::Rectangle<float>(imageBounds.getX() + imageBounds.getWidth() * pointer.drawBoundsNormalized.getX(),
                                                   imageBounds.getY() + imageBounds.getHeight() * pointer.drawBoundsNormalized.getY(),
                                                   imageBounds.getWidth() * pointer.drawBoundsNormalized.getWidth(),
                                                   imageBounds.getHeight() * pointer.drawBoundsNormalized.getHeight());

        const auto centre = juce::Point<float>(imageBounds.getX() + imageBounds.getWidth() * pointer.centerNormalized.x,
                                               imageBounds.getY() + imageBounds.getHeight() * pointer.centerNormalized.y);
        const auto angleRadians = juce::degreesToRadians(getWheelPointerAngleDegrees(pointer));

        {
            juce::Graphics::ScopedSaveState state(g);
            g.addTransform(juce::AffineTransform::rotation(angleRadians, centre.x, centre.y));
            g.drawImage(pointer.image, bounds);
        }

        if (debugWheelPointers)
        {
            g.setColour(getWheelControlColour(pointer.control).withAlpha(0.82f));
            g.drawRect(bounds, 1.0f);
            g.fillEllipse(juce::Rectangle<float>(8.0f, 8.0f).withCentre(centre));
            g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
            g.drawFittedText(getWheelControlLabel(pointer.control) + " "
                                + juce::String(getWheelPointerAngleDegrees(pointer), 1) + " deg",
                             bounds.toNearestInt().reduced(3),
                             juce::Justification::centred,
                             1);
        }
    }
}

void F1Dashboard::paint(juce::Graphics& g)
{
    {
        F1Theme::paintCarbonBackground(g, getLocalBounds());

        const auto imageBounds = getWheelImageBounds();

        // Final Wheel UI: the embedded image is the interface; hotspots below drive hidden APVTS-attached controls.
        if (wheelImage.isValid())
        {
            g.setColour(juce::Colours::black.withAlpha(0.52f));
            g.fillRoundedRectangle(imageBounds.expanded(14.0f).translated(0.0f, 10.0f), 18.0f);
            g.drawImage(wheelImage, imageBounds);
            paintWheelPointers(g, imageBounds);
        }
        else
        {
            g.setColour(juce::Colour(0xff050607));
            g.fillRoundedRectangle(imageBounds, 18.0f);
            g.setColour(F1Theme::red());
            g.drawFittedText("Missing Assets/Wheel/sss33VOLANTE_base.png", imageBounds.toNearestInt(), juce::Justification::centred, 1);
        }

        const auto now = juce::Time::getMillisecondCounter();

        for (const auto& spec : wheelControlMap)
        {
            const auto area = getWheelControlBounds(spec.control);
            const auto accent = getWheelControlColour(spec.control);
            const auto isHovered = spec.control == hoveredWheelControl;
            const auto isActive = spec.control == activeWheelControl;
            const auto isToggled = spec.kind == WheelControlKind::toggle
                                && getWheelParameterId(spec.control).isNotEmpty()
                                && getWheelParameterNormalized(spec.control) >= 0.5f;

            if (debugWheelZones || isHovered || isActive || isToggled)
            {
                const auto isReverbAmount = spec.control == WheelControl::reverbMix;
                const auto fillAlpha = debugWheelZones ? 0.055f
                                     : isActive ? 0.075f
                                     : isToggled ? 0.090f
                                     : isHovered ? 0.024f
                                                 : 0.0f;
                const auto strokeAlpha = debugWheelZones ? 0.28f
                                       : (isActive || isToggled) ? 0.46f
                                                                : 0.20f;

                g.setColour(accent.withAlpha(isReverbAmount && ! isActive && ! debugWheelZones ? fillAlpha * 0.35f
                                                                                                 : fillAlpha));
                if (spec.roundHitZone)
                    g.fillEllipse(area);
                else
                    g.fillRoundedRectangle(area, 10.0f);

                g.setColour(accent.withAlpha(strokeAlpha));
                if (spec.roundHitZone)
                    g.drawEllipse(area.reduced(1.0f), isActive || isToggled ? 1.15f : 0.75f);
                else
                    g.drawRoundedRectangle(area.reduced(1.0f), 9.0f, isActive || isToggled ? 1.05f : 0.7f);
            }
        }

        {
            const auto reverbArea = getWheelControlBounds(WheelControl::reverbMix);
            const auto reverbMix = getParameterNormalizedById("reverbMix");
            const auto showReverbMeter = reverbMix > 0.001f
                                      || hoveredWheelControl == WheelControl::reverbMix
                                      || activeWheelControl == WheelControl::reverbMix
                                      || debugWheelZones;

            if (showReverbMeter && ! reverbArea.isEmpty())
            {
                const auto accent = F1Theme::green();
                const auto activeAmount = activeWheelControl == WheelControl::reverbMix ? 1.0f : 0.0f;
                auto rail = reverbArea.reduced(reverbArea.getWidth() * 0.36f, reverbArea.getHeight() * 0.12f);
                auto fill = rail.withTrimmedTop(rail.getHeight() * (1.0f - juce::jlimit(0.0f, 1.0f, reverbMix)));

                g.setColour(accent.withAlpha(0.16f + activeAmount * 0.12f));
                g.fillRoundedRectangle(fill, 3.0f);
                g.setColour(accent.withAlpha(0.20f + activeAmount * 0.18f));
                g.drawRoundedRectangle(rail, 3.0f, 0.7f);
            }
        }

        auto tooltipControl = activeWheelControl != WheelControl::none ? activeWheelControl : hoveredWheelControl;
        if (tooltipControl != WheelControl::none && (activeWheelControl != WheelControl::none || now < tooltipUntilMs || hoveredWheelControl != WheelControl::none))
        {
            const auto area = getWheelControlBounds(tooltipControl);
            const auto accent = getWheelControlColour(tooltipControl);
            const auto isAirDiagnosticTooltip = tooltipControl == WheelControl::midAir || tooltipControl == WheelControl::highAir;
            auto tooltip = juce::Rectangle<float>(isAirDiagnosticTooltip ? 392.0f : 176.0f,
                                                  isAirDiagnosticTooltip ? 54.0f : 42.0f)
                               .withCentre(area.getCentre().translated(0.0f, -area.getHeight() * 0.64f));
            tooltip.setX(juce::jlimit(imageBounds.getX() + 8.0f, imageBounds.getRight() - tooltip.getWidth() - 8.0f, tooltip.getX()));
            tooltip.setY(juce::jlimit(imageBounds.getY() + 8.0f, imageBounds.getBottom() - tooltip.getHeight() - 8.0f, tooltip.getY()));

            g.setColour(juce::Colours::black.withAlpha(0.64f));
            g.fillRoundedRectangle(tooltip.translated(0.0f, 3.0f), 10.0f);
            g.setColour(juce::Colour(0xee05080b));
            g.fillRoundedRectangle(tooltip, 10.0f);
            g.setColour(accent.withAlpha(0.82f));
            g.drawRoundedRectangle(tooltip.reduced(1.0f), 9.0f, 1.2f);
            g.setColour(F1Theme::text());
            g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
            g.drawFittedText(getWheelControlLabel(tooltipControl),
                             tooltip.toNearestInt().withTrimmedBottom(isAirDiagnosticTooltip ? 30 : 18).reduced(8, 2),
                             juce::Justification::centred,
                             1);
            g.setColour(F1Theme::mutedText());
            g.setFont(juce::FontOptions(isAirDiagnosticTooltip ? 9.0f : 10.0f, juce::Font::bold));
            const auto tooltipValue = (tooltipControl == audibleStatusControl
                                       && now < audibleStatusTooltipUntilMs
                                       && audibleStatusTooltip.isNotEmpty())
                                    ? audibleStatusTooltip
                                    : getWheelControlValueText(tooltipControl);
            g.drawFittedText(tooltipValue,
                             tooltip.toNearestInt().withTrimmedTop(isAirDiagnosticTooltip ? 21 : 20).reduced(8, 1),
                             juce::Justification::centred,
                             isAirDiagnosticTooltip ? 2 : 1);
        }

        if constexpr (debugWheelFunctionalMapping)
        {
            if (debugOverlayText.isNotEmpty() && now < debugOverlayUntilMs)
            {
                auto overlay = juce::Rectangle<float>(330.0f, 92.0f)
                                   .withPosition(imageBounds.getX() + 12.0f,
                                                 imageBounds.getBottom() - 104.0f);
                overlay.setX(juce::jlimit(imageBounds.getX() + 8.0f, imageBounds.getRight() - overlay.getWidth() - 8.0f, overlay.getX()));
                overlay.setY(juce::jlimit(imageBounds.getY() + 8.0f, imageBounds.getBottom() - overlay.getHeight() - 8.0f, overlay.getY()));

                g.setColour(juce::Colours::black.withAlpha(0.62f));
                g.fillRoundedRectangle(overlay.translated(0.0f, 3.0f), 10.0f);
                g.setColour(juce::Colour(0xee040607));
                g.fillRoundedRectangle(overlay, 10.0f);
                g.setColour(F1Theme::cyan().withAlpha(0.78f));
                g.drawRoundedRectangle(overlay.reduced(1.0f), 9.0f, 1.1f);

                auto lines = juce::StringArray::fromLines(debugOverlayText);
                auto textArea = overlay.toNearestInt().reduced(10, 8);
                g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
                for (auto i = 0; i < lines.size(); ++i)
                {
                    g.setColour(i == 0 ? F1Theme::text() : F1Theme::mutedText());
                    g.drawFittedText(lines[i], textArea.removeFromTop(18), juce::Justification::centredLeft, 1);
                }
            }
        }

        return;
    }

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
    paintHubRouteLabel(g, hub.getCentre(), routeRadius, juce::MathConstants<float>::pi * 2.18f, F1Theme::amber(), "OUT");

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
    auto route = juce::String("INPUT > CHANNEL > COMP > AIR > DELAY > OUTPUT");

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

void F1Dashboard::mouseMove(const juce::MouseEvent& event)
{
    const auto control = findWheelControl(event.position);
    if (hoveredWheelControl != control)
    {
        hoveredWheelControl = control;
        setMouseCursor(control == WheelControl::none ? juce::MouseCursor::NormalCursor
                                                     : juce::MouseCursor::PointingHandCursor);
        repaint();
    }
}

void F1Dashboard::mouseExit(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);

    if (hoveredWheelControl != WheelControl::none)
    {
        hoveredWheelControl = WheelControl::none;
        setMouseCursor(juce::MouseCursor::NormalCursor);
        repaint();
    }
}

void F1Dashboard::mouseDown(const juce::MouseEvent& event)
{
    const auto control = findWheelControl(event.position);
    if (control == WheelControl::none)
        return;

    hoveredWheelControl = control;
    setMouseCursor(control == WheelControl::none ? juce::MouseCursor::NormalCursor
                                                 : juce::MouseCursor::PointingHandCursor);

    const auto kind = getWheelControlKind(control);
    if (kind == WheelControlKind::toggle)
    {
        toggleWheelControl(control);
        return;
    }

    if (kind == WheelControlKind::timing || kind == WheelControlKind::stepped)
    {
        beginWheelControlDrag(control, event.position);
        return;
    }

    if (kind == WheelControlKind::placeholder)
    {
        showWheelControlTooltip(control);
        repaint();
        return;
    }

    beginWheelControlDrag(control, event.position);
}

void F1Dashboard::mouseDrag(const juce::MouseEvent& event)
{
    if (activeWheelControl != WheelControl::none)
    {
        setMouseCursor(juce::MouseCursor::DraggingHandCursor);
        dragWheelControl(event.position);
    }
}

void F1Dashboard::mouseUp(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    endWheelControlDrag();
}

void F1Dashboard::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    auto control = findWheelControl(event.position);
    if (control == WheelControl::none)
        control = hoveredWheelControl;

    if (control == WheelControl::none)
        return;

    const auto direction = wheel.deltaY >= 0.0f ? 1 : -1;
    incrementWheelControl(control, direction);
}

void F1Dashboard::resized()
{
    {
        auto setHotspotButton = [this] (GordoButton& button, Hotspot hotspot)
        {
            button.setBounds(getHotspotBounds(hotspot).withSizeKeepingCentre(112.0f, 44.0f).toNearestInt());
        };

        setHotspotButton(tabGlobal, Hotspot::global);
        setHotspotButton(tabChannel, Hotspot::channel);
        setHotspotButton(tabComp, Hotspot::comp);
        setHotspotButton(tabAir, Hotspot::air);
        setHotspotButton(tabDelay, Hotspot::delay);
        setHotspotButton(tabRouting, Hotspot::routing);
        setHotspotButton(tabMeters, Hotspot::meters);

        auto meterArea = (activePage == Page::meters ? getControlPanelBounds().reduced(30.0f, 70.0f)
                                                     : getHotspotBounds(Hotspot::meters).reduced(22.0f, 34.0f)).toNearestInt();
        meterArea = meterArea.withWidth(154).withCentre(meterArea.getCentre());
        inputMeter.setBounds(meterArea.removeFromLeft(48));
        meterArea.removeFromLeft(8);
        outputMeter.setBounds(meterArea.removeFromLeft(48));
        meterArea.removeFromLeft(8);
        gainReductionMeter.setBounds(meterArea.removeFromLeft(48));

        auto panel = getControlPanelBounds().toNearestInt().reduced(10, 6);

        if (activePage == Page::meters)
        {
            panel = {};
        }
        else if (activePage == Page::channel)
        {
            layoutChannelPage(panel);
        }
        else if (activePage == Page::comp)
        {
            layoutCompPage(panel);
        }
        else if (activePage == Page::air)
        {
            layoutAirPage(panel);
        }
        else if (activePage == Page::delay)
        {
            layoutDelayPage(panel);
        }
        else
        {
            layoutGlobalPage(panel);
        }

        auto ledArea = getHotspotBounds(Hotspot::global).withSizeKeepingCentre(250.0f, 20.0f).translated(0.0f, 58.0f).toNearestInt();
        for (auto& led : moduleLeds)
            led.setBounds(ledArea.removeFromLeft(62).withSizeKeepingCentre(18, 18));

        for (auto* tab : tabs)
            tab->toFront(false);

        inputMeter.toFront(false);
        outputMeter.toFront(false);
        gainReductionMeter.toFront(false);
        return;
    }

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
    if (newPage == Page::reverb)
        newPage = Page::global;

    activePage = newPage;
    tabGlobal.setToggleState(activePage == Page::global, juce::dontSendNotification);
    tabChannel.setToggleState(activePage == Page::channel, juce::dontSendNotification);
    tabComp.setToggleState(activePage == Page::comp, juce::dontSendNotification);
    tabAir.setToggleState(activePage == Page::air, juce::dontSendNotification);
    tabDelay.setToggleState(activePage == Page::delay, juce::dontSendNotification);
    tabRouting.setToggleState(activePage == Page::routing, juce::dontSendNotification);
    tabMeters.setToggleState(activePage == Page::meters, juce::dontSendNotification);
    updateControlVisibility();
    resized();
    repaint();
}

void F1Dashboard::updateControlVisibility()
{
    for (auto* knob : { &inputGain, &outputGain, &channelMix, &compMix, &airMix, &delaySend, &masterWidth })
        knob->setVisible(false);

    reverbSend.setVisible(false);

    for (auto* knob : { &channelInputTrim, &channelHighPass, &channelLowPass, &channelLowGain, &channelLowFreq,
                        &channelLowMidGain, &channelLowMidFreq, &channelHighMidGain, &channelHighMidFreq,
                        &channelHighGain, &channelHighFreq, &channelDrive, &channelPageMix })
        knob->setVisible(false);

    for (auto* knob : { &compInput, &compThreshold, &compOutput, &compAttack, &compRelease, &compRatio, &compPageMix,
                        &compSidechainHp, &compRevision, &compNoise })
        knob->setVisible(false);

    for (auto* knob : { &airPageAmount, &airMidAir, &airHighAir, &airFrequency, &airPageTone,
                        &airPageDrive, &airDensity, &airDynamic, &airDeEss, &airPageMix, &airPageOutput })
        knob->setVisible(false);

    for (auto* knob : { &delayTime, &delayDivision, &delayMode, &delayFeedback, &delayPageSend,
                        &delayLeftTime, &delayRightTime, &delayHighPass, &delayLowPass, &delayWidth,
                        &delayLoFi, &delayModDepth, &delayModRate, &delayDucking })
        knob->setVisible(false);

    for (auto* knob : { &reverbPageMix, &reverbPredelay, &reverbDecay, &reverbSize, &reverbAttack, &reverbWidth,
                        &reverbEarly, &reverbLate, &reverbDiffEarly, &reverbDiffLate, &reverbModRate, &reverbModDepth,
                        &reverbLowCut, &reverbHighCut, &reverbLowDamp, &reverbHighDamp, &reverbMode, &reverbColor,
                        &reverbDucking })
        knob->setVisible(false);

    globalBypass.setVisible(false);
    channelEnabled.setVisible(false);
    compEnabled.setVisible(false);
    airEnabled.setVisible(false);
    delayEnabled.setVisible(false);
    reverbEnabled.setVisible(false);
    channelPhaseInvert.setVisible(false);
    compPageEnabled.setVisible(false);
    airPageEnabled.setVisible(false);
    delaySync.setVisible(false);
    delayLink.setVisible(false);
    delayFreeze.setVisible(false);
    delayPageEnabled.setVisible(false);
    reverbFreeze.setVisible(false);
    reverbSyncPredelay.setVisible(false);
    reverbMonoBass.setVisible(false);
    reverbPageEnabled.setVisible(false);

    for (auto* tab : tabs)
        tab->setVisible(false);

    for (auto& led : moduleLeds)
        led.setVisible(false);

    inputMeter.setVisible(false);
    outputMeter.setVisible(false);
    gainReductionMeter.setVisible(false);
}

void F1Dashboard::layoutGlobalPage(juce::Rectangle<int> cockpit)
{
    juce::ignoreUnused(cockpit);

    const auto image = getWheelImageBounds();
    const auto knobW = juce::roundToInt(juce::jlimit(58.0f, 78.0f, image.getWidth() * 0.062f));
    const auto knobH = juce::roundToInt(static_cast<float>(knobW) * 0.95f);
    const auto buttonW = juce::roundToInt(juce::jlimit(58.0f, 78.0f, image.getWidth() * 0.060f));
    const auto buttonH = juce::roundToInt(juce::jlimit(24.0f, 31.0f, image.getHeight() * 0.045f));

    auto at = [&] (float x, float y)
    {
        return juce::Point<int>(juce::roundToInt(image.getX() + image.getWidth() * x),
                                juce::roundToInt(image.getY() + image.getHeight() * y));
    };

    auto placeKnob = [&] (GordoKnob& knob, float x, float y)
    {
        knob.setBounds(juce::Rectangle<int>(knobW, knobH).withCentre(at(x, y)));
    };

    auto placeButton = [&] (GordoButton& button, float x, float y, float widthScale = 1.0f)
    {
        button.setBounds(juce::Rectangle<int>(juce::roundToInt(static_cast<float>(buttonW) * widthScale), buttonH).withCentre(at(x, y)));
    };

    // Global macros stay exposed for host automation and future wheel/controller mapping.
    placeKnob(inputGain, 0.40f, 0.22f);
    placeKnob(outputGain, 0.60f, 0.22f);
    placeKnob(channelMix, 0.34f, 0.39f);
    placeKnob(compMix, 0.63f, 0.39f);
    placeKnob(airMix, 0.34f, 0.62f);
    placeKnob(delaySend, 0.63f, 0.62f);
    placeKnob(masterWidth, 0.50f, 0.56f);
    reverbSend.setBounds(0, 0, 0, 0);

    placeButton(globalBypass, 0.50f, 0.82f, 1.20f);
    placeButton(channelEnabled, 0.28f, 0.31f, 1.18f);
    placeButton(compEnabled, 0.72f, 0.31f, 1.00f);
    placeButton(airEnabled, 0.28f, 0.70f, 0.90f);
    placeButton(delayEnabled, 0.72f, 0.70f, 1.00f);
    reverbEnabled.setBounds(0, 0, 0, 0);
}

void F1Dashboard::layoutChannelPage(juce::Rectangle<int> cockpit)
{
    juce::ignoreUnused(cockpit);

    const auto image = getWheelImageBounds();
    const auto knobW = juce::roundToInt(juce::jlimit(50.0f, 68.0f, image.getWidth() * 0.054f));
    const auto knobH = juce::roundToInt(static_cast<float>(knobW) * 0.96f);
    const auto buttonW = juce::roundToInt(juce::jlimit(54.0f, 76.0f, image.getWidth() * 0.060f));
    const auto buttonH = juce::roundToInt(juce::jlimit(24.0f, 30.0f, image.getHeight() * 0.044f));

    auto at = [&] (float x, float y)
    {
        return juce::Point<int>(juce::roundToInt(image.getX() + image.getWidth() * x),
                                juce::roundToInt(image.getY() + image.getHeight() * y));
    };

    auto placeKnob = [&] (GordoKnob& knob, float x, float y)
    {
        knob.setBounds(juce::Rectangle<int>(knobW, knobH).withCentre(at(x, y)));
    };

    placeKnob(channelInputTrim, 0.28f, 0.27f);
    placeKnob(channelHighPass, 0.35f, 0.26f);
    placeKnob(channelLowPass, 0.42f, 0.27f);
    placeKnob(channelDrive, 0.30f, 0.39f);
    placeKnob(channelPageMix, 0.42f, 0.40f);
    placeKnob(channelLowGain, 0.27f, 0.52f);
    placeKnob(channelLowFreq, 0.34f, 0.52f);
    placeKnob(channelLowMidGain, 0.41f, 0.52f);
    placeKnob(channelLowMidFreq, 0.48f, 0.52f);
    placeKnob(channelHighMidGain, 0.27f, 0.65f);
    placeKnob(channelHighMidFreq, 0.34f, 0.65f);
    placeKnob(channelHighGain, 0.41f, 0.65f);
    placeKnob(channelHighFreq, 0.48f, 0.65f);

    channelPhaseInvert.setBounds(juce::Rectangle<int>(buttonW, buttonH).withCentre(at(0.22f, 0.40f)));
}

void F1Dashboard::layoutCompPage(juce::Rectangle<int> cockpit)
{
    juce::ignoreUnused(cockpit);

    const auto image = getWheelImageBounds();
    const auto knobW = juce::roundToInt(juce::jlimit(50.0f, 68.0f, image.getWidth() * 0.054f));
    const auto knobH = juce::roundToInt(static_cast<float>(knobW) * 0.96f);
    const auto buttonW = juce::roundToInt(juce::jlimit(48.0f, 68.0f, image.getWidth() * 0.055f));
    const auto buttonH = juce::roundToInt(juce::jlimit(24.0f, 30.0f, image.getHeight() * 0.044f));

    auto at = [&] (float x, float y)
    {
        return juce::Point<int>(juce::roundToInt(image.getX() + image.getWidth() * x),
                                juce::roundToInt(image.getY() + image.getHeight() * y));
    };

    auto placeKnob = [&] (GordoKnob& knob, float x, float y)
    {
        knob.setBounds(juce::Rectangle<int>(knobW, knobH).withCentre(at(x, y)));
    };

    placeKnob(compInput, 0.55f, 0.27f);
    placeKnob(compThreshold, 0.62f, 0.26f);
    placeKnob(compOutput, 0.69f, 0.27f);
    placeKnob(compAttack, 0.55f, 0.40f);
    placeKnob(compRelease, 0.62f, 0.40f);
    placeKnob(compRatio, 0.69f, 0.40f);
    placeKnob(compPageMix, 0.55f, 0.54f);
    placeKnob(compSidechainHp, 0.62f, 0.54f);
    placeKnob(compRevision, 0.69f, 0.54f);
    placeKnob(compNoise, 0.76f, 0.54f);

    compPageEnabled.setBounds(juce::Rectangle<int>(buttonW, buttonH).withCentre(at(0.77f, 0.39f)));
}

void F1Dashboard::layoutAirPage(juce::Rectangle<int> cockpit)
{
    juce::ignoreUnused(cockpit);

    const auto image = getWheelImageBounds();
    const auto knobW = juce::roundToInt(juce::jlimit(48.0f, 66.0f, image.getWidth() * 0.052f));
    const auto knobH = juce::roundToInt(static_cast<float>(knobW) * 0.96f);
    const auto buttonW = juce::roundToInt(juce::jlimit(48.0f, 66.0f, image.getWidth() * 0.052f));
    const auto buttonH = juce::roundToInt(juce::jlimit(24.0f, 30.0f, image.getHeight() * 0.044f));

    auto at = [&] (float x, float y)
    {
        return juce::Point<int>(juce::roundToInt(image.getX() + image.getWidth() * x),
                                juce::roundToInt(image.getY() + image.getHeight() * y));
    };

    auto placeKnob = [&] (GordoKnob& knob, float x, float y)
    {
        knob.setBounds(juce::Rectangle<int>(knobW, knobH).withCentre(at(x, y)));
    };

    placeKnob(airPageAmount, 0.25f, 0.50f);
    placeKnob(airMidAir, 0.32f, 0.50f);
    placeKnob(airHighAir, 0.39f, 0.50f);
    placeKnob(airFrequency, 0.46f, 0.50f);
    placeKnob(airPageDrive, 0.25f, 0.63f);
    placeKnob(airDensity, 0.32f, 0.63f);
    placeKnob(airDynamic, 0.39f, 0.63f);
    placeKnob(airDeEss, 0.46f, 0.63f);
    placeKnob(airPageTone, 0.30f, 0.76f);
    placeKnob(airPageMix, 0.38f, 0.76f);
    placeKnob(airPageOutput, 0.46f, 0.76f);

    airPageEnabled.setBounds(juce::Rectangle<int>(buttonW, buttonH).withCentre(at(0.20f, 0.64f)));
}

void F1Dashboard::layoutDelayPage(juce::Rectangle<int> cockpit)
{
    juce::ignoreUnused(cockpit);

    const auto image = getWheelImageBounds();
    const auto knobW = juce::roundToInt(juce::jlimit(46.0f, 62.0f, image.getWidth() * 0.050f));
    const auto knobH = juce::roundToInt(static_cast<float>(knobW) * 0.96f);
    const auto buttonW = juce::roundToInt(juce::jlimit(44.0f, 64.0f, image.getWidth() * 0.050f));
    const auto buttonH = juce::roundToInt(juce::jlimit(22.0f, 29.0f, image.getHeight() * 0.042f));

    auto at = [&] (float x, float y)
    {
        return juce::Point<int>(juce::roundToInt(image.getX() + image.getWidth() * x),
                                juce::roundToInt(image.getY() + image.getHeight() * y));
    };

    auto placeKnob = [&] (GordoKnob& knob, float x, float y)
    {
        knob.setBounds(juce::Rectangle<int>(knobW, knobH).withCentre(at(x, y)));
    };

    auto placeButton = [&] (GordoButton& button, float x, float y, float widthScale = 1.0f)
    {
        button.setBounds(juce::Rectangle<int>(juce::roundToInt(static_cast<float>(buttonW) * widthScale), buttonH).withCentre(at(x, y)));
    };

    placeKnob(delayTime, 0.54f, 0.50f);
    placeButton(delaySync, 0.60f, 0.46f, 1.05f);
    placeKnob(delayDivision, 0.62f, 0.50f);
    placeKnob(delayMode, 0.70f, 0.50f);
    placeKnob(delayFeedback, 0.76f, 0.50f);
    placeKnob(delayPageSend, 0.76f, 0.63f);
    placeKnob(delayLeftTime, 0.54f, 0.63f);
    placeKnob(delayRightTime, 0.62f, 0.63f);
    placeButton(delayLink, 0.68f, 0.61f, 1.00f);
    placeKnob(delayHighPass, 0.70f, 0.63f);
    placeKnob(delayLowPass, 0.54f, 0.76f);
    placeKnob(delayWidth, 0.62f, 0.76f);
    placeKnob(delayLoFi, 0.70f, 0.76f);
    placeKnob(delayModDepth, 0.76f, 0.76f);
    placeKnob(delayModRate, 0.58f, 0.86f);
    placeKnob(delayDucking, 0.66f, 0.86f);
    placeButton(delayFreeze, 0.73f, 0.86f, 1.14f);
    placeButton(delayPageEnabled, 0.80f, 0.86f, 0.90f);
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
