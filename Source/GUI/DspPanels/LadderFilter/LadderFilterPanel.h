#pragma once

#include "../../ParameterComponents/ParameterComponent.h"
#include <JuceHeader.h>

class LadderFilterPanel : public juce::Component {
public:
  LadderFilterPanel(juce::AudioProcessorValueTreeState &apvts);
  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<std::unique_ptr<ParameterComponent>> controls;
};
