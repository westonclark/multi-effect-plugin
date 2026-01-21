#pragma once

#include "../../Parameters.h"
#include <JuceHeader.h>

class PercentSlider : public juce::Slider {
public:
  juce::String getTextFromValue(double value) override {
    int percent = juce::roundToInt(value * 100.0);
    if (percent == 0)
      return "0" + getTextValueSuffix();
    return juce::String(percent) + getTextValueSuffix();
  }

  double getValueFromText(const juce::String &text) override {
    return text.getDoubleValue() / 100.0;
  }
};

struct SliderWithLabel {
  std::unique_ptr<juce::Slider> slider;
  std::unique_ptr<juce::Label> label;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      attachment;

  SliderWithLabel(const Parameter &param,
                  juce::AudioProcessorValueTreeState &apvts,
                  juce::Component *parent);

  static void layoutHorizontally(juce::Rectangle<int> bounds,
                                 std::vector<SliderWithLabel> &sliders);
};
