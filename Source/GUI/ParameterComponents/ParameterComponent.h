#pragma once

#include "../../Parameters.h"
#include <JuceHeader.h>

class ParameterComponent {
public:
  virtual ~ParameterComponent() = default;
  virtual void setBounds(juce::Rectangle<int> bounds) = 0;

  static std::unique_ptr<ParameterComponent>
  create(const Parameter &parameter, juce::AudioProcessorValueTreeState &apvts,
         juce::Component *component);

  static void layoutHorizontally(
      juce::Rectangle<int> bounds,
      std::vector<std::unique_ptr<ParameterComponent>> &components);

protected:
  ParameterComponent() = default;
};
