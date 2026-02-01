#pragma once

#include "../ParameterComponent.h"

class ParameterToggle : public ParameterComponent {
public:
  ParameterToggle(const Parameter &parameter,
                  juce::AudioProcessorValueTreeState &apvts,
                  juce::Component *component);

  void setBounds(juce::Rectangle<int> bounds) override;

private:
  Parameter parameter;
  juce::Component *component;

  std::unique_ptr<juce::ToggleButton> toggleButton;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
      buttonAttachment;
};
