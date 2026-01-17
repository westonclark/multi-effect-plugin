#pragma once

#include <JuceHeader.h>

// PHASER PANEL
//==============================================================================
class PhaserPanel : public juce::Component {
public:
  PhaserPanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;
};
