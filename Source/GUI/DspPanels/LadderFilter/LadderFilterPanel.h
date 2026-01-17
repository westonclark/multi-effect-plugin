#pragma once

#include <JuceHeader.h>

// LADDER FILTER PANEL
//==============================================================================
class LadderFilterPanel : public juce::Component {
public:
  LadderFilterPanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;
};
