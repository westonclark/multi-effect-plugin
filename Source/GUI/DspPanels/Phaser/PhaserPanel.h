#pragma once

#include <JuceHeader.h>

// PHASER PANEL
//==============================================================================
class PhaserPanel : public juce::Component {
public:
  PhaserPanel(juce::AudioProcessorValueTreeState &apvts);
  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;

  // static inline const char *rate = "Phaser Rate";
  // static inline const char *depth = "Phaser Depth";
  // static inline const char *centerFreq = "Phaser Center Freq";
  // static inline const char *feedback = "Phaser Feedback";
  // static inline const char *mix = "Phaser Mix";
  // static inline const char *bypass = "Phaser Bypass";

  // juce::Slider[] parameters{rate, depth, center, feedback, mix, bypass};
  juce::Slider rate;
  // juce::Slider depth;
  // juce::Slider center;
  // juce::Slider feedback;
  // juce::Slider mix;
  // juce::ToggleButton bypass;
};
