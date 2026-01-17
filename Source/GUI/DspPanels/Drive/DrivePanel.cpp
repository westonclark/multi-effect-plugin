#include "DrivePanel.h"

// DRIVE PANEL
//==============================================================================
DrivePanel::DrivePanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {}

void DrivePanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::white);
  g.setFont(20.0f);
  g.drawText("Drive", getLocalBounds(), juce::Justification::centred);
}
