#pragma once

#include "../../LookAndFeel.h"
#include "../AudioMeter/AudioMeter.h"
#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class Output : public juce::Component {
public:
  Output(juce::AudioProcessorValueTreeState &apvts,
         AudioMeterFifo<MeterLevel> &outputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;

  std::unique_ptr<ParameterComponent> outputSlider;
  AudioMeter outputMeter;
  juce::Label label;
};
