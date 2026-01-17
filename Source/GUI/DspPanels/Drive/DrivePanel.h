#pragma once

#include <JuceHeader.h>

// DRIVE PANEL
//==============================================================================
class DrivePanel : public juce::Component {
public:
  DrivePanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;
};
