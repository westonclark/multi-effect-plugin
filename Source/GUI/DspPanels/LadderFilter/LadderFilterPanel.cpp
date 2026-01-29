#include "LadderFilterPanel.h"

LadderFilterPanel::LadderFilterPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  for (const auto &param : Parameters::LadderFilter::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void LadderFilterPanel::paint(juce::Graphics &g) {}

void LadderFilterPanel::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
