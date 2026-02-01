#include "ChorusPanel.h"

ChorusPanel::ChorusPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  for (const auto &param : Parameters::Chorus::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void ChorusPanel::paint(juce::Graphics &g) {}

void ChorusPanel::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
