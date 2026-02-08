#pragma once

#include "../../../Utils/Fifos/InputOutputLevelFifo.h"
#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class AudioMeter : public juce::Component, juce::Timer {
public:
  AudioMeter(InputOutputLevelFifo<std::vector<float>> &inputOutputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  InputOutputLevelFifo<std::vector<float>> &inputOutputLevelFifo;
  std::vector<float> level = {0.0f, 0.0f};

  void timerCallback() override;
};
