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

void PhaserPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText("Phaser", getLocalBounds(), juce::Justification::centred);

  int borderThickness = 2;
  g.setColour(juce::Colours::darkgrey);
  g.drawRect(getLocalBounds().reduced(borderThickness / 2), borderThickness);
}

void PhaserPanel::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
