#pragma once

#include "../../../Utils/Fifos/InputLevelFifo.h"
#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class Input : public juce::Component, juce::Timer {
public:
  Input(juce::AudioProcessorValueTreeState &apvts,
        InputLevelFifo<std::vector<float>> &inputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::vector<std::unique_ptr<ParameterComponent>> controls;

  InputLevelFifo<std::vector<float>> &inputLevelFifo;
  std::vector<float> inputLevel = {0.0f, 0.0f};

  void timerCallback() override;
};
