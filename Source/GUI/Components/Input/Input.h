#pragma once

#include "../../../Utils/Fifos/InputOutputLevelFifo.h"
#include "../../LookAndFeel.h"
#include "../AudioMeter/AudioMeter.h"
#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class Input : public juce::Component {
public:
  Input(juce::AudioProcessorValueTreeState &apvts,
        InputOutputLevelFifo<std::vector<float>> &inputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::unique_ptr<ParameterComponent> inputSlider;
  AudioMeter inputMeter;
  juce::Label label;
};
