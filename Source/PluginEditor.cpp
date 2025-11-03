/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

static juce::String
getDSPOptionName(MultieffectpluginAudioProcessor::DSP_Option dspOption) {
  switch (dspOption) {
  case MultieffectpluginAudioProcessor::DSP_Option::Phase:
    return "Phaser";
    break;
  case MultieffectpluginAudioProcessor::DSP_Option::Chorus:
    return "Chorus";
    break;
  case MultieffectpluginAudioProcessor::DSP_Option::OverDrive:
    return "Drive";
    break;
  case MultieffectpluginAudioProcessor::DSP_Option::LadderFilter:
    return "Ladder Filter";
    break;
  case MultieffectpluginAudioProcessor::DSP_Option::Filter:
    return "Filter";
    break;
  case MultieffectpluginAudioProcessor::DSP_Option::END_OF_LIST:
    jassertfalse;
    break;
  }
  return "None Selected";
};
ExtendedTabBarButton::ExtendedTabBarButton(const juce::String &name,
                                           juce::TabbedButtonBar &owner)
    : juce::TabBarButton(name, owner) {};

juce::TabBarButton *
ExtendedTabbedButtonBar::createTabButton(const juce::String &tabName,
                                         int tabIndex) {
  return new ExtendedTabBarButton(tabName, *this);
};

//==============================================================================
MultieffectpluginAudioProcessorEditor::MultieffectpluginAudioProcessorEditor(
    MultieffectpluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  dspOrderButton.onClick = [this]() {
    juce::Random random;
    MultieffectpluginAudioProcessor::DSP_Order dspOrder;

    auto range = juce::Range<int>(
        static_cast<int>(MultieffectpluginAudioProcessor::DSP_Option::Phase),
        static_cast<int>(
            MultieffectpluginAudioProcessor::DSP_Option::END_OF_LIST));

    tabbedComponent.clearTabs();

    for (auto &dspOption : dspOrder) {
      auto entry = random.nextInt(range);
      dspOption =
          static_cast<MultieffectpluginAudioProcessor::DSP_Option>(entry);
      tabbedComponent.addTab(getDSPOptionName(dspOption), juce::Colours::white,
                             -1);
    }

    DBG(juce::Base64::toBase64(dspOrder.data(), dspOrder.size()));

    audioProcessor.dspOrderFifo.push(dspOrder);
  };

  addAndMakeVisible(dspOrderButton);
  addAndMakeVisible(tabbedComponent);
  setSize(400, 300);
}

MultieffectpluginAudioProcessorEditor::
    ~MultieffectpluginAudioProcessorEditor() {}

//==============================================================================
void MultieffectpluginAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background
  // with a solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
  g.drawFittedText("Hello World!", getLocalBounds(),
                   juce::Justification::centred, 1);
}

void MultieffectpluginAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  dspOrderButton.setBounds(
      bounds.removeFromTop(30).withSizeKeepingCentre(150, 30));
  bounds.removeFromTop(10);
  tabbedComponent.setBounds(bounds.removeFromTop(30));
}
