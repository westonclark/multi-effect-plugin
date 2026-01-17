#include "PhaserPanel.h"

// PHASER PANEL
//==============================================================================
PhaserPanel::PhaserPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {}

void PhaserPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::white);
  g.setFont(20.0f);
  g.drawText("Phaser", getLocalBounds(), juce::Justification::centred);
}
