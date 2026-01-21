#pragma once

#include "../../Sliders/Sliders.h"
#include <JuceHeader.h>

class ChorusPanel : public juce::Component {
public:
  ChorusPanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<SliderWithLabel> sliders;
};
