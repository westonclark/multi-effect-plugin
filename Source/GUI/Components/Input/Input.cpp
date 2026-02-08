#include "Input.h"

Input::Input(juce::AudioProcessorValueTreeState &apvts,
             InputOutputLevelFifo<std::vector<float>> &inputLevelFifo)
    : apvts(apvts), inputMeter(inputLevelFifo) {
  for (const auto &param : Parameters::Input::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
}

void Input::paint(juce::Graphics &g) { addAndMakeVisible(inputMeter); }

void Input::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
  inputMeter.setBounds(getLocalBounds().removeFromTop(80));
}
