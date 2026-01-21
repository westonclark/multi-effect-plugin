#include "ChorusPanel.h"

ChorusPanel::ChorusPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  for (const auto &param : Parameters::Chorus::sliderParams) {
    sliders.push_back(SliderWithLabel(param, apvts, this));
  }
}

void ChorusPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText("Chorus", getLocalBounds(), juce::Justification::centred);

  int borderThickness = 2;
  g.setColour(juce::Colours::darkgrey);
  g.drawRect(getLocalBounds().reduced(borderThickness / 2), borderThickness);
}

void ChorusPanel::resized() {
  SliderWithLabel::layoutHorizontally(getLocalBounds(), sliders);
}
