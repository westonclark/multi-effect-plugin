#include "Input.h"

Input::Input(juce::AudioProcessorValueTreeState &apvts,
             InputLevelFifo<std::vector<float>> &inputLevelFifo)
    : apvts(apvts), inputLevelFifo(inputLevelFifo) {
  for (const auto &param : Parameters::Input::params) {
    if (param.type == ParameterType::Bool)
      continue;
    controls.push_back(ParameterComponent::create(param, apvts, this));
  }
  startTimerHz(60);
}

void Input::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText(juce::String(juce::Decibels::gainToDecibels(inputLevel[0])) +
                 " dB",
             getLocalBounds(), juce::Justification::centred);
}

void Input::resized() {
  ParameterComponent::layoutHorizontally(getLocalBounds(), controls);
}

void Input::timerCallback() {
  // Pull input level from fifo
  std::vector<float> newInputLevel;
  while (inputLevelFifo.pull(newInputLevel)) {
    inputLevel = newInputLevel;
  };

  repaint();
};
