#include "LadderFilterPanel.h"

// LADDER FILTER PANEL
//==============================================================================
LadderFilterPanel::LadderFilterPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {}

void LadderFilterPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::white);
  g.setFont(20.0f);
  g.drawText("Ladder Filter", getLocalBounds(), juce::Justification::centred);
}
