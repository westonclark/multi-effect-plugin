#include "Output.h"

Output::Output(juce::AudioProcessorValueTreeState &apvts,
               InputOutputLevelFifo<std::vector<float>> &outputLevelFifo)
    : apvts(apvts), outputMeter(outputLevelFifo) {
  outputSlider =
      ParameterComponent::create(Parameters::Output::gain, apvts, this, false);
  addAndMakeVisible(outputMeter);

  // Label
  label.setText("Output", juce::dontSendNotification);
  label.setJustificationType(juce::Justification::centred);
  label.setFont(juce::Font(14.0f, juce::Font::bold));
  addAndMakeVisible(label);
}

void Output::paint(juce::Graphics &g) {
  // Border
  LookAndFeel::drawBorder(g, getLookAndFeel(), getLocalBounds());
}

void Output::resized() {
  auto bounds = getLocalBounds().reduced(4);

  // Label
  label.setBounds(bounds.removeFromTop(24));
  bounds.removeFromTop(4);

  // Knob
  auto knobHeight = bounds.getHeight() * 0.35f;
  outputSlider->setBounds(bounds.removeFromBottom(knobHeight));

  // Meter
  bounds.removeFromBottom(5);
  outputMeter.setBounds(bounds);
}
