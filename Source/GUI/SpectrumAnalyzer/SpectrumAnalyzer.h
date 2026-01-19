#pragma once

#include <JuceHeader.h>

// SPECTRUM ANALYZER
//==============================================================================
class SpectrumAnalyzer : public juce::Component {
public:
  SpectrumAnalyzer();
  void resized() override;
  void paint(juce::Graphics &g) override;

private:
};
