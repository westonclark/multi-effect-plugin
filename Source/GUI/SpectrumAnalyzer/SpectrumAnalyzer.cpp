#include "SpectrumAnalyzer.h"

// SPECTRUM ANALYZER
//==============================================================================
SpectrumAnalyzer::SpectrumAnalyzer() {};

void SpectrumAnalyzer::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText("Spectrum Analyzer", getLocalBounds(),
             juce::Justification::centred);

  int borderThickness = 2;
  g.setColour(juce::Colours::darkgrey);
  g.drawRect(getLocalBounds().reduced(borderThickness / 2), borderThickness);
}

void SpectrumAnalyzer::resized() {
  auto bounds = getLocalBounds();
  // addAndMakeVisible(border);
}
