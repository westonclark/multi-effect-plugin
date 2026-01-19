#include "LadderFilterPanel.h"

// LADDER FILTER PANEL
//==============================================================================
LadderFilterPanel::LadderFilterPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {}

void LadderFilterPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText("Ladder Filter", getLocalBounds(), juce::Justification::centred);

  int borderThickness = 2;
  g.setColour(juce::Colours::darkgrey);
  g.drawRect(getLocalBounds().reduced(borderThickness / 2), borderThickness);
}
