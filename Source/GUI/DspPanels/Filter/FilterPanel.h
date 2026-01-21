#pragma once

#include "../../Sliders/Sliders.h"
#include <JuceHeader.h>
#include <vector>

class FilterPanel : public juce::Component {
public:
  FilterPanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<SliderWithLabel> sliders;
};
