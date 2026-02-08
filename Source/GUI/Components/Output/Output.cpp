#include "Output.h"

Output::Output(juce::AudioProcessorValueTreeState &apvts,
               InputOutputLevelFifo<std::vector<float>> &outputLevelFifo)
    : apvts(apvts), outputMeter(outputLevelFifo) {
  for (const auto &param : Parameters::Output::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void Output::paint(juce::Graphics &g) { addAndMakeVisible(outputMeter); }

void Output::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
  outputMeter.setBounds(getLocalBounds().removeFromTop(80));
}
