#pragma once

#include <JuceHeader.h>

class PowerButton : public juce::ToggleButton {
public:
  PowerButton() {
    setSize(20, 20);
    setClickingTogglesState(true);
  }

  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
    auto &lnf = getLookAndFeel();

    auto localBounds = getLocalBounds().toFloat();
    auto size = juce::jmin(localBounds.getWidth(), localBounds.getHeight());
    auto bounds = localBounds.withSizeKeepingCentre(size, size).reduced(2.0f);
    auto centre = bounds.getCentre();

    bool isActive = !getToggleState();
    ;

    auto baseThumbColor = lnf.findColour(juce::Slider::thumbColourId);
    auto baseBgColor =
        lnf.findColour(juce::ResizableWindow::backgroundColourId);

    juce::Colour symbolColor =
        isActive ? baseThumbColor : baseThumbColor.darker(0.7f);

    juce::Colour bgColor =
        isActive ? baseBgColor.brighter(0.2f) : baseBgColor.darker(0.4f);

    // Circle background
    g.setColour(bgColor);
    g.fillEllipse(bounds);

    // Circle border
    g.setColour(symbolColor);
    g.drawEllipse(bounds, 1.5f);

    // Vertical line + arc
    auto lineTop = centre.withY(bounds.getY() + 4.0f);
    auto lineBottom = centre.withY(centre.y - 1.0f);
    g.setColour(symbolColor);
    g.drawLine(lineTop.x, lineTop.y, lineBottom.x, lineBottom.y, 2.0f);

    juce::Path arc;
    auto arcRadius = (bounds.getWidth() / 2.0f) - 4.0f;
    g.setColour(symbolColor);
    arc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                      juce::MathConstants<float>::pi * 0.135f, // Start
                      juce::MathConstants<float>::pi * 1.865f, // End
                      true);
    g.strokePath(arc, juce::PathStrokeType(2.0f));

    if (shouldDrawButtonAsHighlighted) {
      g.setColour(lnf.findColour(juce::Label::textColourId).withAlpha(0.2f));
      g.fillEllipse(bounds);
    }
  }
};
