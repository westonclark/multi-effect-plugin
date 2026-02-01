#pragma once

#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class PhaserPanel : public juce::Component {
public:
  PhaserPanel(juce::AudioProcessorValueTreeState &apvts);
  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<std::unique_ptr<ParameterComponent>> controls;
};
