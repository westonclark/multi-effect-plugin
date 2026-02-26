#pragma once

#include "../../../Utils/Fifos/AudioMeterFifo.h"
#include "../../../Utils/Fifos/SpectrumAnalyzerFifo.h"
#include "../../LookAndFeel.h"
#include "../AudioMeter/AudioMeter.h"
#include "../ParameterControls/ParameterComponent.h"
#include <JuceHeader.h>

class Input : public juce::Component {
public:
  Input(juce::AudioProcessorValueTreeState &apvts,
        AudioMeterFifo<std::vector<float>> &inputLevelFifo);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::AudioProcessorValueTreeState &apvts;
  std::unique_ptr<ParameterComponent> inputSlider;
  AudioMeter inputMeter;
  juce::Label label;
};
