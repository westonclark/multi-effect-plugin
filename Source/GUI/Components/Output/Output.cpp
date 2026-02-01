#include "Output.h"

Output::Output(juce::AudioProcessorValueTreeState &apvts) : apvts(apvts) {
  for (const auto &param : Parameters::Output::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void Output::paint(juce::Graphics &g) {}

void Output::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
