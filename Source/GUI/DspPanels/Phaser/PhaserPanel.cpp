#include "PhaserPanel.h"
#include "../../../Parameters.h"

PhaserPanel::PhaserPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  for (const auto &param : Parameters::Phaser::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void PhaserPanel::paint(juce::Graphics &g) {}

void PhaserPanel::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
