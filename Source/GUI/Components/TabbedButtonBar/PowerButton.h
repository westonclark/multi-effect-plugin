#pragma once

#include <JuceHeader.h>

class PowerButton : public juce::ToggleButton {
public:
  PowerButton() {
    setSize(15, 15);
    setClickingTogglesState(true);
  }

  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
    auto &lnf = getLookAndFeel();

    auto localBounds = getLocalBounds().toFloat();
    auto centre = localBounds.getCentre();
    auto radius =
        juce::jmin(localBounds.getWidth(), localBounds.getHeight()) / 2.0f -
        1.0f;

    bool isActive = !getToggleState();
    auto activeColor = lnf.findColour(juce::Slider::thumbColourId);

    g.setColour(activeColor.withAlpha(0.6f));
    g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f,
                  radius * 2.0f, 1.5f);

    if (isActive) {
      g.setColour(activeColor);
      g.fillEllipse(centre.x - radius + 2.0f, centre.y - radius + 2.0f,
                    (radius - 2.0f) * 2.0f, (radius - 2.0f) * 2.0f);
    }

    if (shouldDrawButtonAsHighlighted) {
      g.setColour(lnf.findColour(juce::Label::textColourId).withAlpha(0.1f));
      g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f,
                    radius * 2.0f);
    }
  }
};

class PaddedPowerButton : public juce::Component {
public:
  PaddedPowerButton(PowerButton *button) : powerButton(button) {
    addAndMakeVisible(powerButton);
    setSize(19, 28);
    setInterceptsMouseClicks(false, true);
  }

  void resized() override {
    auto bounds = getLocalBounds();

    int leftPadding = bounds.getWidth() - 15;
    powerButton->setBounds(leftPadding, (bounds.getHeight() - 15) / 2, 15, 15);
  }

private:
  juce::Component::SafePointer<PowerButton> powerButton;
};
