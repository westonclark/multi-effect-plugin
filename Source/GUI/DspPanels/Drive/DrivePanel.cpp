#include "DrivePanel.h"

// DRIVE PANEL
//==============================================================================
DrivePanel::DrivePanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {}

void DrivePanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::grey);
  g.setFont(20.0f);
  g.drawText("Drive", getLocalBounds(), juce::Justification::centred);

  int borderThickness = 2;
  g.setColour(juce::Colours::darkgrey);
  g.drawRect(getLocalBounds().reduced(borderThickness / 2), borderThickness);
}
