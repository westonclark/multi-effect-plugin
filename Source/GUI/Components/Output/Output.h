#pragma once

#include "../../../Utils/Fifos/InputOutputLevelFifo.h"
#include "../ParameterControls/ParameterComponent.h"
#include "../AudioMeter/AudioMeter.h"
#include <JuceHeader.h>

class Output : public juce::Component {
public:
  Output(juce::AudioProcessorValueTreeState &apvts,
         InputOutputLevelFifo<std::vector<float>> &outputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<std::unique_ptr<ParameterComponent>> controls;

	AudioMeter outputMeter;
};
