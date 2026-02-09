#include "Input.h"

Input::Input(juce::AudioProcessorValueTreeState &apvts,
             InputOutputLevelFifo<std::vector<float>> &inputLevelFifo)
    : apvts(apvts), inputMeter(inputLevelFifo) {
  inputSlider =
      ParameterComponent::create(Parameters::Input::gain, apvts, this);
  addAndMakeVisible(inputMeter);
}

void Input::paint(juce::Graphics &g) {}

void Input::resized() {
  auto bounds = getLocalBounds();
  inputSlider->setBounds(bounds.removeFromBottom(150));
  inputMeter.setBounds(bounds);
}
