#pragma once

#include <JuceHeader.h>

// CHORUS PANEL
//==============================================================================
class ChorusPanel : public juce::Component {
public:
  ChorusPanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;
};
