#include "Input.h"

Input::Input(juce::AudioProcessorValueTreeState &apvts) : apvts(apvts) {
  for (const auto &param : Parameters::Input::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void Input::paint(juce::Graphics &g) {}

void Input::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
