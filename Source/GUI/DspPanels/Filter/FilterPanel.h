#pragma once

#include <JuceHeader.h>

// FILTER PANEL
//==============================================================================
class FilterPanel : public juce::Component {
public:
  FilterPanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;
};
