#include "Output.h"

Output::Output(juce::AudioProcessorValueTreeState &apvts,
               InputOutputLevelFifo<std::vector<float>> &outputLevelFifo)
    : apvts(apvts), outputMeter(outputLevelFifo) {
  outputSlider =
      ParameterComponent::create(Parameters::Output::gain, apvts, this);
  addAndMakeVisible(outputMeter);
}

void Output::paint(juce::Graphics &g) {}

void Output::resized() {
  auto bounds = getLocalBounds();
  outputSlider->setBounds(bounds.removeFromBottom(150));
  outputMeter.setBounds(bounds);
}
