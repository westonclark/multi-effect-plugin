#pragma once

#include "../../Sliders/Sliders.h"
#include <JuceHeader.h>

class PhaserPanel : public juce::Component {
public:
  PhaserPanel(juce::AudioProcessorValueTreeState &apvts);
  void resized() override;
  void paint(juce::Graphics &g) override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<SliderWithLabel> sliders;
};
