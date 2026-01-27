#include "ParameterToggle.h"

ParameterToggle::ParameterToggle(const Parameter &parameter,
                                 juce::AudioProcessorValueTreeState &apvts,
                                 juce::Component *component)
    : parameter(parameter), component(component) {

  toggleButton = std::make_unique<juce::ToggleButton>(parameter.displayName);

  buttonAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
          apvts, parameter.id, *toggleButton);

  component->addAndMakeVisible(*toggleButton);
}

void ParameterToggle::setBounds(juce::Rectangle<int> bounds) {
  toggleButton->setBounds(bounds);
}
