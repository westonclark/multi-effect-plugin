#pragma once

#include "../ParameterComponents/ParameterComponent.h"
#include <JuceHeader.h>

class Output : public juce::Component {
public:
  Output(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<std::unique_ptr<ParameterComponent>> controls;
};
