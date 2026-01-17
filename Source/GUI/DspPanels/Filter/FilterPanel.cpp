#include "FilterPanel.h"

// FILTER PANEL
//==============================================================================
FilterPanel::FilterPanel(juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts) {}

void FilterPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colours::white);
  g.setFont(20.0f);
  g.drawText("Filter", getLocalBounds(), juce::Justification::centred);
}
