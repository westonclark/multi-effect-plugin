#include "FilterPanel.h"

FilterPanel::FilterPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  for (const auto &param : Parameters::Filter::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void FilterPanel::paint(juce::Graphics &g) {}

void FilterPanel::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
