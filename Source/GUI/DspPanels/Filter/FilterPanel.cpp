#include "FilterPanel.h"

FilterPanel::FilterPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  for (const auto &param : Parameters::Filter::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void FilterPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText("Filter", getLocalBounds(), juce::Justification::centred);

  int borderThickness = 2;
  g.setColour(juce::Colours::darkgrey);
  g.drawRect(getLocalBounds().reduced(borderThickness / 2), borderThickness);
}

void FilterPanel::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}
