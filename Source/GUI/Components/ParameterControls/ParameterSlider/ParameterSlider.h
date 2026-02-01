#pragma once

#include "../ParameterComponent.h"

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

class ParameterSlider : public ParameterComponent {
public:
  ParameterSlider(const Parameter &parameter,
                  juce::AudioProcessorValueTreeState &apvts,
                  juce::Component *component);

  void setBounds(juce::Rectangle<int> bounds) override;

private:
  Parameter parameter;
  juce::Component *component;

  std::unique_ptr<juce::Slider> slider;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      sliderAttachment;
  std::unique_ptr<juce::Label> label;
};
