#include "Output.h"

Output::Output(juce::AudioProcessorValueTreeState &apvts,
               OutputLevelFifo<std::vector<float>> &outputLevelFifo)
    : apvts(apvts), outputLevelFifo(outputLevelFifo) {
  for (const auto &param : Parameters::Output::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
  startTimerHz(60);
}

void Output::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText(juce::String(juce::Decibels::gainToDecibels(outputLevel[0])) +
                 " dB",
             getLocalBounds(), juce::Justification::centred);
}

void Output::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}

void Output::timerCallback() {
  // Pull output level from fifo
  std::vector<float> newOutputLevel;
  while (outputLevelFifo.pull(newOutputLevel)) {
    outputLevel = newOutputLevel;
  };

  repaint();
};
