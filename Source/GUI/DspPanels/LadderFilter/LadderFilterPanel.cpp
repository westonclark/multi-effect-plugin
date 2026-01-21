#include "LadderFilterPanel.h"

LadderFilterPanel::LadderFilterPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  for (const auto &param : Parameters::LadderFilter::sliderParams) {
    sliders.push_back(SliderWithLabel(param, apvts, this));
  }
}

void LadderFilterPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText("Ladder Filter", getLocalBounds(), juce::Justification::centred);

  int borderThickness = 2;
  g.setColour(juce::Colours::darkgrey);
  g.drawRect(getLocalBounds().reduced(borderThickness / 2), borderThickness);
}

void LadderFilterPanel::resized() {
  SliderWithLabel::layoutHorizontally(getLocalBounds(), sliders);
}
