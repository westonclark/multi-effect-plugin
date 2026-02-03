#pragma once

#include "../../../Utils/Fifos/OutputLevelFifo.h"
#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class Output : public juce::Component, juce::Timer {
public:
  Output(juce::AudioProcessorValueTreeState &apvts,
         OutputLevelFifo<std::vector<float>> &outputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<std::unique_ptr<ParameterComponent>> controls;

  OutputLevelFifo<std::vector<float>> &outputLevelFifo;
  std::vector<float> outputLevel = {0.0f, 0.0f};

  void timerCallback() override;
};
