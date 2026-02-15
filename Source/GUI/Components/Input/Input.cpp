#include "Input.h"

Input::Input(juce::AudioProcessorValueTreeState &apvts,
             InputOutputLevelFifo<std::vector<float>> &inputLevelFifo)
    : apvts(apvts), inputMeter(inputLevelFifo) {
  inputSlider =
      ParameterComponent::create(Parameters::Input::gain, apvts, this, false);
  addAndMakeVisible(inputMeter);

  // Label
  label.setText("Input", juce::dontSendNotification);
  label.setJustificationType(juce::Justification::centred);
  label.setFont(juce::Font(14.0f, juce::Font::bold));
  addAndMakeVisible(label);
}

void Input::paint(juce::Graphics &g) {
  // Border
  LookAndFeel::drawBorder(g, getLookAndFeel(), getLocalBounds());
}

void Input::resized() {
  auto bounds = getLocalBounds().reduced(4);

  // Label
  label.setBounds(bounds.removeFromTop(24));
  bounds.removeFromTop(4); // Gap

  // Knob
  auto knobHeight = bounds.getHeight() * 0.35f;
  inputSlider->setBounds(bounds.removeFromBottom(knobHeight));

  // Meter
  bounds.removeFromBottom(5);
  inputMeter.setBounds(bounds);
}
