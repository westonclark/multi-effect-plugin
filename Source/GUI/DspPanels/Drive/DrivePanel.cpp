#include "DrivePanel.h"

DrivePanel::DrivePanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  for (const auto &param : Parameters::Overdrive::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void DrivePanel::paint(juce::Graphics &g) {}

void DrivePanel::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
