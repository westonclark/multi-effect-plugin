#pragma once

#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class ChorusPanel : public juce::Component {
public:
  ChorusPanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<std::unique_ptr<ParameterComponent>> controls;
};
