#include "PhaserPanel.h"

// PHASER PANEL
//==============================================================================
PhaserPanel::PhaserPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {
  rate.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  rate.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
  addAndMakeVisible(rate);
}

void PhaserPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText("Phaser", getLocalBounds(), juce::Justification::centred);

  int borderThickness = 2;
  g.setColour(juce::Colours::darkgrey);
  g.drawRect(getLocalBounds().reduced(borderThickness / 2), borderThickness);
}

void PhaserPanel::resized() {
  auto bounds = getLocalBounds();
  rate.setBounds(bounds.removeFromLeft(80));
}
