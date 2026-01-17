#include "ChorusPanel.h"

// CHORUS PANEL
//==============================================================================
ChorusPanel::ChorusPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {}

void ChorusPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::white);
  g.setFont(20.0f);
  g.drawText("Chorus", getLocalBounds(), juce::Justification::centred);
}
