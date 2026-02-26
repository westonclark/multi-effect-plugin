#pragma once

#include "../../../Utils/Fifos/AudioMeterFifo.h"
#include "../../LookAndFeel.h"
#include "../AudioMeter/AudioMeter.h"
#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class Output : public juce::Component {
public:
  Output(juce::AudioProcessorValueTreeState &apvts,
         AudioMeterFifo<std::vector<float>> &outputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;

  std::unique_ptr<ParameterComponent> outputSlider;
  AudioMeter outputMeter;
  juce::Label label;
};
