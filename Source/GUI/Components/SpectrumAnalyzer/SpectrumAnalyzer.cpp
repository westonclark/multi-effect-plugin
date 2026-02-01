#include "SpectrumAnalyzer.h"

// SPECTRUM ANALYZER
//==============================================================================
SpectrumAnalyzer::SpectrumAnalyzer() {};

void SpectrumAnalyzer::paint(juce::Graphics &g) {
  auto &lnf = getLookAndFeel();

  g.setColour(lnf.findColour(juce::Label::textColourId));
  g.setFont(20.0f);
  g.drawText("Spectrum Analyzer", getLocalBounds(),
             juce::Justification::centred);
}

void SpectrumAnalyzer::resized() { auto bounds = getLocalBounds(); }
