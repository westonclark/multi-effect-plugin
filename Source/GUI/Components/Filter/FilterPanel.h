#pragma once

#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class FilterPanel : public juce::Component {
public:
  FilterPanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<std::unique_ptr<ParameterComponent>> controls;
};
