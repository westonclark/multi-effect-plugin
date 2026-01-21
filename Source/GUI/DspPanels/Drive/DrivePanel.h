#pragma once

#include "../../Sliders/Sliders.h"
#include <JuceHeader.h>

class DrivePanel : public juce::Component {
public:
  DrivePanel(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<SliderWithLabel> sliders;
};
